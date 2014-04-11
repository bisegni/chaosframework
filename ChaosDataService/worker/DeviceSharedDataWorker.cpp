//
//  DeviceDataWorker.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 06/03/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#include "DeviceSharedDataWorker.h"
#include <chaos/common/utility/ObjectFactoryRegister.h>

using namespace chaos::data_service::worker;

DeviceSharedDataWorker::DeviceSharedDataWorker(std::string _cache_impl_name):cache_impl_name(_cache_impl_name) {
	
}

DeviceSharedDataWorker::~DeviceSharedDataWorker() {
}

void DeviceSharedDataWorker::init(void *init_data) throw (chaos::CException) {
	DataWorker::init(init_data);
	for(int idx = 0; idx < settings.job_thread_number; idx++) {
		thread_cookie[idx] = chaos::ObjectFactoryRegister<cache_system::CacheDriver>::getInstance()->getNewInstanceByName(cache_impl_name.c_str());
	}
}

void DeviceSharedDataWorker::deinit() throw (chaos::CException) {
	for(int idx = 0; idx < settings.job_thread_number; idx++) {
		cache_system::CacheDriver *tmp_ptr = reinterpret_cast<cache_system::CacheDriver*>(thread_cookie[idx]);
		delete(tmp_ptr);
	}
	std::memset(thread_cookie, 0, sizeof(void*)*settings.job_thread_number);
	DataWorker::deinit();
}

void DeviceSharedDataWorker::executeJob(WorkerJobPtr job_info, void* cookie) {
	DeviceSharedWorkerJob *job_ptr = reinterpret_cast<DeviceSharedWorkerJob*>(job_info);
	//check what kind of push we have
	switch(job_ptr->request_header->tag) {
		case 0:// storicize only
			break;
			

		case 2:// storicize and live
			//punt into history stream
			
		case 1:// live only only
			cache_system::CacheDriver *cache_driver_instance = reinterpret_cast<cache_system::CacheDriver*>(cookie);
			cache_driver_instance->putData(GET_PUT_OPCODE_KEY_PTR(job_ptr->request_header),
										   job_ptr->request_header->key_len,
										   job_ptr->data_pack,
										   job_ptr->data_pack_len);
			free(job_ptr->request_header);
			free(job_ptr->data_pack);
			free(job_info);
			break;
	}
}


void DeviceSharedDataWorker::addServer(std::string server_description) {
	for(int idx = 0; idx < settings.job_thread_number; idx++) {
		cache_system::CacheDriver *tmp_ptr = reinterpret_cast<cache_system::CacheDriver*>(thread_cookie[idx]);
		if(tmp_ptr) tmp_ptr->addServer(server_description);
	}

}

void DeviceSharedDataWorker::updateServerConfiguration() {
	for(int idx = 0; idx < settings.job_thread_number; idx++) {
		cache_system::CacheDriver *tmp_ptr = reinterpret_cast<cache_system::CacheDriver*>(thread_cookie[idx]);
		if(tmp_ptr) tmp_ptr->updateConfig();
	}
}