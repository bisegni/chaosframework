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

using namespace chaos::common::metric;
using namespace chaos::metadata_service::worker;

DeviceSharedDataWorkerMetricCollector::DeviceSharedDataWorkerMetricCollector():
DeviceSharedDataWorker(){
    MetricManager::getInstance()->createCounterFamily("mds_dataset_received", "Is the number of dataset received from mds processing layer");
    counter_dataset_in_uptr = MetricManager::getInstance()->getNewCounterFromFamily("mds_dataset_received");
    
    MetricManager::getInstance()->createGaugeFamily("mds_dataset_storage_queue", "Is the metric for storage queue");
    gauge_queued_dataset_uptr = MetricManager::getInstance()->getNewGaugeFromFamily("mds_dataset_storage_queue",{{"type","queued_dataset_number"}});
    gauge_queued_memory_uptr = MetricManager::getInstance()->getNewGaugeFromFamily("mds_dataset_storage_queue",{{"type","queued_dataset_memeory"}});
}

DeviceSharedDataWorkerMetricCollector::~DeviceSharedDataWorkerMetricCollector() {}

void DeviceSharedDataWorkerMetricCollector::executeJob(WorkerJobPtr job_info,
                                                       void* cookie) {
    DeviceSharedWorkerJob& job = *reinterpret_cast<DeviceSharedWorkerJob*>(job_info.get());
    int tag = job.key_tag;
    uint32_t total_data = (uint32_t)job.data_pack->size() + (uint32_t)job.key.size();
    DeviceSharedDataWorker::executeJob(job_info, cookie);
    switch(tag) {
        case 0:// storicize only
        case 2:// storicize and live
            //increment metric for data stored on vfs
            (*gauge_queued_dataset_uptr)--;
            (*gauge_queued_memory_uptr)-=total_data;
            break;
            
        case 1:{// live only
            break;
        }
    }
    //decrement metric that this packet has been removed from queue
    (*gauge_queued_dataset_uptr)--;
}

int DeviceSharedDataWorkerMetricCollector::submitJobInfo(WorkerJobPtr job_info) {
    int err = 0;
    DeviceSharedWorkerJob& job = *reinterpret_cast<DeviceSharedWorkerJob*>(job_info.get());
    int tag = job.key_tag;
    uint32_t total_data = (uint32_t)job.data_pack->size()  + (uint32_t)job.key.size();
    (*counter_dataset_in_uptr)++;
    
    switch(tag) {
        case 0:// storicize only
        case 2:// storicize and live
            //write data on stage file
            (*gauge_queued_dataset_uptr)++;
            (*gauge_queued_memory_uptr)+=total_data;
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
