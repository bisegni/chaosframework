/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */

#include "DeviceSharedDataWorkerMetricCollector.h"

#include "../ChaosMetadataService.h"

#include <chaos/common/global.h>

using namespace chaos::data_service::worker;

DeviceSharedDataWorkerMetricCollector::DeviceSharedDataWorkerMetricCollector(ChaosSharedPtr<DeviceSharedDataWorkerMetric> _data_worker_metric):
DeviceSharedDataWorker(),
data_worker_metric(_data_worker_metric){}

DeviceSharedDataWorkerMetricCollector::~DeviceSharedDataWorkerMetricCollector() {}

void DeviceSharedDataWorkerMetricCollector::executeJob(WorkerJobPtr job_info,
                                                       void* cookie) {
    int tag = reinterpret_cast<DeviceSharedWorkerJob*>(job_info)->request_header->tag;
    uint32_t total_data = reinterpret_cast<DeviceSharedWorkerJob*>(job_info)->data_pack_len + reinterpret_cast<DeviceSharedWorkerJob*>(job_info)->request_header->key_len;
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
    //decrement metric that this packet has been removed from queue
    data_worker_metric->decrementQueueSize(total_data);
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
