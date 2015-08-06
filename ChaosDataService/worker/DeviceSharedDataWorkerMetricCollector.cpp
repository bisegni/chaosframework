/*
 *	DeviceSharedDataWorkerMetricCollector.cpp
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

#include "DeviceSharedDataWorkerMetricCollector.h"

#include "../ChaosDataService.h"

#include <chaos/common/global.h>

using namespace chaos::data_service::worker;

DeviceSharedDataWorkerMetricCollector::DeviceSharedDataWorkerMetricCollector(const std::string& _cache_impl_name,
                                                                             vfs::VFSManager *_vfs_manager_instance,
                                                                             boost::shared_ptr<DeviceSharedDataWorkerMetric> _data_worker_metric):
DeviceSharedDataWorker(_cache_impl_name,
                       _vfs_manager_instance),
data_worker_metric(_data_worker_metric){}

DeviceSharedDataWorkerMetricCollector::~DeviceSharedDataWorkerMetricCollector() {}

void DeviceSharedDataWorkerMetricCollector::executeJob(WorkerJobPtr job_info,
                                                       void* cookie) {
    int tag = reinterpret_cast<DeviceSharedWorkerJob*>(job_info)->request_header->tag;
    uint32_t total_data = reinterpret_cast<DeviceSharedWorkerJob*>(job_info)->data_pack_len + reinterpret_cast<DeviceSharedWorkerJob*>(job_info)->request_header->key_len;
    //increment metric that has been removed from queue
    data_worker_metric->decrementQueueSize(total_data);
    DeviceSharedDataWorker::executeJob(job_info, cookie);
    switch(tag) {
        case 0:// storicize only
        case 2:// storicize and live
            //increment metric for data stored on vfs
            data_worker_metric->incrementOutputBandwith(total_data);
            break;
            
        case 1:{// live only
            break;
        }
    }
}

int DeviceSharedDataWorkerMetricCollector::submitJobInfo(WorkerJobPtr job_info) {
    int err = 0;
    int tag = reinterpret_cast<DeviceSharedWorkerJob*>(job_info)->request_header->tag;
    uint32_t total_data = reinterpret_cast<DeviceSharedWorkerJob*>(job_info)->data_pack_len + reinterpret_cast<DeviceSharedWorkerJob*>(job_info)->request_header->key_len;
    data_worker_metric->incrementInputBandwith(total_data);
    
    switch(tag) {
        case 0:// storicize only
        case 2:// storicize and live
            //write data on stage file
            data_worker_metric->incrementQueueSize(total_data);
            break;
            
        case 1:{// live only
           
            break;
        }
    }
    err = DeviceSharedDataWorker::submitJobInfo(job_info);
    switch(tag) {
        case 0:// storicize only
        case 2:// storicize and live
            //write data on stage file
           
            break;
            
        case 1:{// live only
            break;
        }
    }
    return err;
}