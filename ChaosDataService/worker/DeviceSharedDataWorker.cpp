/*
 *	DeviceDataWorker.cpp
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2014 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
 */

#include "../ChaosDataService.h"
#include "DeviceSharedDataWorker.h"
#include "../cache_system/CacheDriverMetricCollector.h"
#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/utility/TimingUtil.h>
#include <chaos/common/data/cache/FastHash.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>

#include <boost/lexical_cast.hpp>
using namespace chaos::common::utility;
using namespace chaos::data_service::worker;
namespace chaos_vfs = chaos::data_service::vfs;

#define DSDW_LAPP_ INFO_LOG(DeviceSharedDataWorker)
#define DSDW_LDBG_ DBG_LOG(DeviceSharedDataWorker)
#define DSDW_LERR_ ERR_LOG(DeviceSharedDataWorker)


//------------------------------------------------------------------------------------------------------------------------

DeviceSharedDataWorker::DeviceSharedDataWorker(const std::string& _cache_impl_name,
											   vfs::VFSManager *_vfs_manager_instance):
cache_impl_name(_cache_impl_name),
vfs_manager_instance(_vfs_manager_instance),
cache_driver_ptr(NULL) {}

DeviceSharedDataWorker::~DeviceSharedDataWorker() {}

void DeviceSharedDataWorker::init(void *init_data) throw (chaos::CException) {
	DataWorker::init(init_data);
	
	last_stage_file_hb = 0;
	
	//generate random path for this worker
	std::string path(UUIDUtil::generateUUIDLite());
	
	DSDW_LAPP_ << "allocating cache driver for every thread";
	if(vfs_manager_instance) {
		//we have the file manager configure
		for(int idx = 0; idx < settings.job_thread_number; idx++) {
			std::string stage_file_name("/thread_");
			stage_file_name.append(boost::lexical_cast<std::string>(idx));
			
			ThreadCookie *_tc_ptr = new ThreadCookie();
			if(vfs_manager_instance->getWriteableStageFile(path+stage_file_name, &_tc_ptr->vfs_stage_file)) {
				//we have got an error
				DSDW_LERR_ << "Error getting vfs file for " << path+stage_file_name;
				throw chaos::CException(-2, "Error creating vfs stage file", __PRETTY_FUNCTION__);
			}
			//associate the thread cooky for the idx value
			thread_cookie[idx] = _tc_ptr;
		}
	}
	cache_driver_ptr = ObjectFactoryRegister<cache_system::CacheDriver>::getInstance()->getNewInstanceByName(cache_impl_name);
    if(ChaosDataService::getInstance()->setting.cache_driver_setting.log_metric) {
        DSDW_LDBG_ << "Enable cache log metric";
        cache_driver_ptr = new cache_system::CacheDriverMetricCollector(cache_driver_ptr);
    }
	InizializableService::initImplementation(cache_driver_ptr, &ChaosDataService::getInstance()->setting.cache_driver_setting, "CacheDriver", __PRETTY_FUNCTION__);
}

void DeviceSharedDataWorker::deinit() throw (chaos::CException) {
	if(vfs_manager_instance) {
		for(int idx = 0; idx < settings.job_thread_number; idx++) {
			ThreadCookie *tmp_cookie = reinterpret_cast<ThreadCookie *>(thread_cookie[idx]);
			vfs_manager_instance->releaseFile(tmp_cookie->vfs_stage_file);
		}
		
		std::memset(thread_cookie, 0, sizeof(void*)*settings.job_thread_number);
	}
	
	if(cache_driver_ptr) {
		try {
			InizializableService::deinitImplementation(cache_driver_ptr, "CacheDriver", __PRETTY_FUNCTION__);
		} catch(...) {}
		delete(cache_driver_ptr);
	}
	DataWorker::deinit();
}

void DeviceSharedDataWorker::executeJob(WorkerJobPtr job_info, void* cookie) {
	int err = 0;
	DeviceSharedWorkerJob *job_ptr = reinterpret_cast<DeviceSharedWorkerJob*>(job_info);
	ThreadCookie *this_thread_cookie = reinterpret_cast<ThreadCookie *>(cookie);
    
    CHAOS_ASSERT(job_ptr)
    CHAOS_ASSERT(job_ptr->request_header);
    CHAOS_ASSERT(job_ptr->data_pack);
    CHAOS_ASSERT(this_thread_cookie)
    
	//check what kind of push we have
	//read lock on mantainance mutex
	boost::shared_lock<boost::shared_mutex> rl(this_thread_cookie->mantainance_mutex);
	switch(job_ptr->request_header->tag) {
		case 0:// storicize only
		case 2:// storicize and live
			//write data on stage file
            CHAOS_ASSERT(job_ptr->data_pack)
			CHAOS_ASSERT(this_thread_cookie->vfs_stage_file)
			if((err = this_thread_cookie->vfs_stage_file->write(job_ptr->data_pack, job_ptr->data_pack_len))) {
				DSDW_LERR_<< "Error writing data to file " << this_thread_cookie->vfs_stage_file->getVFSFileInfo()->vfs_fpath;
			}
			
            //check for timeout
			if((TimingUtil::getTimeStamp() - last_stage_file_hb) > 1000) {
				last_stage_file_hb = TimingUtil::getTimeStamp();
				if((err = this_thread_cookie->vfs_stage_file->giveHeartbeat())){
					DSDW_LERR_<< "Error giving heartbeat to data "  << this_thread_cookie->vfs_stage_file->getVFSFileInfo()->vfs_fpath;
				}
			}
			
			free(job_ptr->request_header);
			free(job_ptr->data_pack);
			free(job_ptr);
			break;
			
		case 1:{// live only
			break;
		}
	}
}


void DeviceSharedDataWorker::addServer(std::string server_description) {
	cache_driver_ptr->addServer(server_description);
}

void DeviceSharedDataWorker::updateServerConfiguration() {
	cache_driver_ptr->updateConfig();
}

int DeviceSharedDataWorker::submitJobInfo(WorkerJobPtr job_info) {
	int err = 0;
    if(!job_info) return 0;
	DeviceSharedWorkerJob *job_ptr = reinterpret_cast<DeviceSharedWorkerJob*>(job_info);
    CHAOS_ASSERT(job_ptr->request_header)
    CHAOS_ASSERT(job_ptr->data_pack)
    CHAOS_ASSERT(cache_driver_ptr)
    bool to_delete = (job_ptr->request_header->tag == 1);
	switch(job_ptr->request_header->tag) {
		case 0:// storicize only
			err = DataWorker::submitJobInfo(job_info);
			break;
			
		case 2:// storicize and live
			cache_driver_ptr->putData(GET_PUT_OPCODE_KEY_PTR(job_ptr->request_header),
									  job_ptr->request_header->key_len,
									  job_ptr->data_pack,
									  job_ptr->data_pack_len);
			//if we have the file manager configure we can push the job for write data in storage workfloy
			err = DataWorker::submitJobInfo(job_info);
			break;
			
		case 1:{// live only only
			cache_driver_ptr->putData(GET_PUT_OPCODE_KEY_PTR(job_ptr->request_header),
									  job_ptr->request_header->key_len,
									  job_ptr->data_pack,
									  job_ptr->data_pack_len);
			break;
		}
		default: {
			DSDW_LERR_ << "Bad storage tag: " << job_ptr->request_header->tag;
			break;
		}
	}
	
	if(to_delete) {
		free(job_ptr->request_header);
		free(job_ptr->data_pack);
		free(job_info);
	}
	return err;
}

//!
void DeviceSharedDataWorker::mantain() throw (chaos::CException) {
	// lock for mantains
	for(int idx = 0; idx < settings.job_thread_number; idx++) {
		ThreadCookie *current_tread_cookie = reinterpret_cast<ThreadCookie *>(thread_cookie[idx]);
        CHAOS_ASSERT(current_tread_cookie)
		//write lock on mantainance mutex
		boost::unique_lock<boost::shared_mutex> rl(current_tread_cookie->mantainance_mutex);
		
		//mantainance on virtual file
		current_tread_cookie->vfs_stage_file->mantain();
	}
}