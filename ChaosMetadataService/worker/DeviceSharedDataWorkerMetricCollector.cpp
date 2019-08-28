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
    MetricManager::getInstance()->createGaugeFamily("mds_storage_queue", "Metrics for storage mds queue, element in queue are waiting to be porcessed by object storage driver");
    gauge_queued_dataset_uptr = MetricManager::getInstance()->getNewGaugeFromFamily("mds_storage_queue",{{"type","queued_element"}});
    gauge_queued_memory_uptr = MetricManager::getInstance()->getNewGaugeFromFamily("mds_storage_queue",{{"type","queued_data"}});
}

DeviceSharedDataWorkerMetricCollector::~DeviceSharedDataWorkerMetricCollector() {}

void DeviceSharedDataWorkerMetricCollector::executeJob(WorkerJobPtr job_info,
                                                       void* cookie) {
    DeviceSharedWorkerJob& job = *reinterpret_cast<DeviceSharedWorkerJob*>(job_info.get());
    uint32_t total_data = (uint32_t)job.data_pack->size() + (uint32_t)job.key.size();
    DeviceSharedDataWorker::executeJob(job_info, cookie);
    switch(static_cast<DataServiceNodeDefinitionType::DSStorageType>(job.key_tag)) {
        case DataServiceNodeDefinitionType::DSStorageTypeHistory:// storicize only
        case DataServiceNodeDefinitionType::DSStorageTypeLiveHistory:{// storicize and live
            (*gauge_queued_dataset_uptr)--;
            (*gauge_queued_memory_uptr)-=total_data;
            break;
        }
        case DataServiceNodeDefinitionType::DSStorageTypeLive:
        case DataServiceNodeDefinitionType::DSStorageTypeUndefined:{// live only
            break;
        }
    }
}

int DeviceSharedDataWorkerMetricCollector::submitJobInfo(WorkerJobPtr job_info, int64_t milliseconds_to_wait) {
    DeviceSharedWorkerJob& job = *reinterpret_cast<DeviceSharedWorkerJob*>(job_info.get());
    uint32_t total_data = (uint32_t)job.data_pack->size()  + (uint32_t)job.key.size();
    
    switch(static_cast<DataServiceNodeDefinitionType::DSStorageType>(job.key_tag)) {
        case DataServiceNodeDefinitionType::DSStorageTypeHistory:// storicize only
        case DataServiceNodeDefinitionType::DSStorageTypeLiveHistory:{// storicize and live
            (*gauge_queued_dataset_uptr)++;
            (*gauge_queued_memory_uptr)+=total_data;
            break;
        }
        case DataServiceNodeDefinitionType::DSStorageTypeLive:
        case DataServiceNodeDefinitionType::DSStorageTypeUndefined:{// live only
            break;
        }
    }
    return DeviceSharedDataWorker::submitJobInfo(job_info);
}
