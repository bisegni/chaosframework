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

#include "../ChaosMetadataService.h"
#include "DeviceSharedDataWorker.h"
#include "../cache_system/CacheDriverMetricCollector.h"
#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/utility/TimingUtil.h>
#include <chaos/common/data/cache/FastHash.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>

#include <boost/lexical_cast.hpp>
using namespace chaos::common::utility;
using namespace chaos::metadata_service;
using namespace chaos::metadata_service::worker;
using namespace chaos::service_common::persistence::data_access;
using namespace chaos::metadata_service::object_storage::abstraction;


#define INFO INFO_LOG(DeviceSharedDataWorker)
#define DBG DBG_LOG(DeviceSharedDataWorker)
#define ERR ERR_LOG(DeviceSharedDataWorker)


//------------------------------------------------------------------------------------------------------------------------

DeviceSharedDataWorker::DeviceSharedDataWorker(){}

DeviceSharedDataWorker::~DeviceSharedDataWorker() {}

void DeviceSharedDataWorker::init(void *init_data)  {
    DataWorker::init(init_data);
    
    const std::string object_impl_name = ChaosMetadataService::getInstance()->setting.object_storage_setting.driver_impl;
    INFO << CHAOS_FORMAT("Allocating object storage driver '%1%' for every worker thread", %object_impl_name);
    
    global_object_storage_driver.reset(ObjectFactoryRegister<AbstractPersistenceDriver>::getInstance()->getNewInstanceByName(object_impl_name+"ObjectStorageDriver"));
    global_object_storage_driver->init(init_data);
    ObjectStorageDataAccess *obj_storage_da  = global_object_storage_driver->getDataAccess<ObjectStorageDataAccess>();
    
    for(int idx = 0; idx < ChaosMetadataService::getInstance()->setting.worker_setting.thread_number; idx++) {
        //associate the thread cooky for the idx value
        thread_cookie[idx] = (void*)obj_storage_da;
    }
}

void DeviceSharedDataWorker::deinit()  {
    DataWorker::deinit();
    global_object_storage_driver->deinit();
}

void DeviceSharedDataWorker::executeJob(WorkerJobPtr job_info, void* cookie) {
    int err = 0;
    DeviceSharedWorkerJob &job = *reinterpret_cast<DeviceSharedWorkerJob*>(job_info.get());
    ObjectStorageDataAccess *obj_storage_da = reinterpret_cast<ObjectStorageDataAccess *>(cookie);
    
    CHAOS_ASSERT(job.data_pack);
    CHAOS_ASSERT(obj_storage_da);
    
    //check what kind of push we have
    //read lock on mantainance mutex
    switch(static_cast<DataServiceNodeDefinitionType::DSStorageType>(job.key_tag)) {
        case DataServiceNodeDefinitionType::DSStorageTypeHistory:// storicize only
        case DataServiceNodeDefinitionType::DSStorageTypeLiveHistory:{// storicize and live
            //write data on object storage
            CDataWrapper data_pack((char *)job.data_pack->data());
            //push received datapack into object storage
            if((err = obj_storage_da->pushObject(job.key,
                                                 MOVE(job.meta_tag),
                                                 data_pack))) {
                ERR << "Error pushing datapack into object storage driver";
            }
            break;
        }
        case DataServiceNodeDefinitionType::DSStorageTypeLive:
        case DataServiceNodeDefinitionType::DSStorageTypeUndefined:{// live only
            break;
        }
    }
}
