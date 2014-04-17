//
//  DeviceDataWorker.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 06/03/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#include "DeviceSharedDataWorker.h"
#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/data/cache/FastHash.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>

#include <boost/lexical_cast.hpp>

using namespace chaos::data_service::worker;
namespace chaos_vfs = chaos::data_service::vfs;

#define DeviceSharedDataWorker_LOG_HEAD "[DeviceSharedDataWorker] - "
#define DSDW_LAPP_ LAPP_ << DeviceSharedDataWorker_LOG_HEAD
#define DSDW_LDBG_ LDBG_ << DeviceSharedDataWorker_LOG_HEAD << __FUNCTION__ << " - "
#define DSDW_LERR_ LERR_ << DeviceSharedDataWorker_LOG_HEAD << __FUNCTION__ << " - "


//------------------------------------------------------------------------------------------------------------------------

DeviceSharedDataWorker::DeviceSharedDataWorker(std::string _cache_impl_name, vfs::VFSManager *_vfs_manager_instance):
cache_impl_name(_cache_impl_name),
vfs_manager_instance(_vfs_manager_instance) {
	
}

DeviceSharedDataWorker::~DeviceSharedDataWorker() {
}

void DeviceSharedDataWorker::init(void *init_data) throw (chaos::CException) {
	DataWorker::init(init_data);
	std::string path("stage/");
	
	//geenrate random path for this worker
	path.append(UUIDUtil::generateUUIDLite());
	
	DSDW_LAPP_ << "Generated worker stage path " << path;
	
	//create path for this instance of worker
	if(vfs_manager_instance->createDirectory(path, true)) {
		DSDW_LERR_ << "Error creating thread folder  " << path;
		throw chaos::CException(-2, "Error creating thread folder", __PRETTY_FUNCTION__);
	}
	
	DSDW_LAPP_ << "allocating cache driver for every thread";
	for(int idx = 0; idx < settings.job_thread_number; idx++) {
		std::string stage_file_name("/thread_");
		stage_file_name.append(boost::lexical_cast<std::string>(idx));
		
		
		
		ThreadCookie *_tc_ptr = new ThreadCookie();
		if(vfs_manager_instance->getFile(path+stage_file_name, &_tc_ptr->vfs_stage_file)) {
			//we have got an error
			DSDW_LERR_ << "Error getting vfs file for " << path+stage_file_name;
			throw chaos::CException(-2, "Error creating vfs stage file", __PRETTY_FUNCTION__);
		}
		
		_tc_ptr->cache_driver_ptr = chaos::ObjectFactoryRegister<cache_system::CacheDriver>::getInstance()->getNewInstanceByName(cache_impl_name.c_str());
		
		//associate the thread cooky for the idx value
		thread_cookie[idx] = _tc_ptr;
	}
}

void DeviceSharedDataWorker::deinit() throw (chaos::CException) {
	for(int idx = 0; idx < settings.job_thread_number; idx++) {
		ThreadCookie *tmp_cookie = reinterpret_cast<ThreadCookie *>(thread_cookie[idx]);
		delete(tmp_cookie->cache_driver_ptr);
		vfs_manager_instance->releaseFile(tmp_cookie->vfs_stage_file);
	}
	std::memset(thread_cookie, 0, sizeof(void*)*settings.job_thread_number);
	
	DataWorker::deinit();
}

void DeviceSharedDataWorker::executeJob(WorkerJobPtr job_info, void* cookie) {
	DeviceSharedWorkerJob *job_ptr = reinterpret_cast<DeviceSharedWorkerJob*>(job_info);
	ThreadCookie *this_thread_cookie = reinterpret_cast<ThreadCookie *>(cookie);
	//check what kind of push we have
	
	switch(job_ptr->request_header->tag) {
		case 0:// storicize only
			//
			this_thread_cookie->vfs_stage_file->write(job_ptr->data_pack, job_ptr->data_pack_len);
			free(job_ptr->request_header);
			free(job_ptr->data_pack);
			free(job_info);
			break;
			
		case 2:// storicize and live
			this_thread_cookie->cache_driver_ptr->putData(GET_PUT_OPCODE_KEY_PTR(job_ptr->request_header),
														  job_ptr->request_header->key_len,
														  job_ptr->data_pack,
														  job_ptr->data_pack_len);
			this_thread_cookie->vfs_stage_file->write(job_ptr->data_pack, job_ptr->data_pack_len);
			free(job_ptr->request_header);
			free(job_ptr->data_pack);
			free(job_info);
			break;
			
		case 1:{// live only only
			this_thread_cookie->cache_driver_ptr->putData(GET_PUT_OPCODE_KEY_PTR(job_ptr->request_header),
														  job_ptr->request_header->key_len,
														  job_ptr->data_pack,
														  job_ptr->data_pack_len);
			free(job_ptr->request_header);
			free(job_ptr->data_pack);
			free(job_info);
			break;
		}
	}
}


void DeviceSharedDataWorker::addServer(std::string server_description) {
	for(int idx = 0; idx < settings.job_thread_number; idx++) {
		ThreadCookie *this_thread_cookie = reinterpret_cast<ThreadCookie *>(thread_cookie[idx]);
		if(this_thread_cookie) this_thread_cookie->cache_driver_ptr->addServer(server_description);
	}
	
}

void DeviceSharedDataWorker::updateServerConfiguration() {
	for(int idx = 0; idx < settings.job_thread_number; idx++) {
		ThreadCookie *this_thread_cookie = reinterpret_cast<ThreadCookie *>(thread_cookie[idx]);
		if(this_thread_cookie) this_thread_cookie->cache_driver_ptr->updateConfig();
	}
}