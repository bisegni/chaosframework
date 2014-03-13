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

DeviceSharedDataWorker::DeviceSharedDataWorker(std::string cache_impl_name):cache_driver_instance(NULL) {
	cache_driver_instance = chaos::ObjectFactoryRegister<cache_system::CacheDriver>::getInstance()->getNewInstanceByName(cache_impl_name.c_str());
	
}

DeviceSharedDataWorker::~DeviceSharedDataWorker() {
	if(cache_driver_instance) delete(cache_driver_instance);
}

void DeviceSharedDataWorker::executeJob(WorkerJobPtr job_info) {
	DeviceSharedWorkerJob *job_ptr = reinterpret_cast<DeviceSharedWorkerJob*>(job_info);
	cache_driver_instance->putData(job_ptr->device_hash, job_ptr->data_pack, job_ptr->data_pack_len);
	free(job_ptr->data_pack);
	free(job_info);
}

void DeviceSharedDataWorker::addServer(std::string server_description) {
	cache_driver_instance->addServer(server_description);
}