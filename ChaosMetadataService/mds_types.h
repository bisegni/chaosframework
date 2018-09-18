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
#ifndef CHAOSFramework_mds_types_h
#define CHAOSFramework_mds_types_h

#include <map>
#include <vector>
#include <string>

#include "batch/mds_service_batch.h"
#include "persistence/persistence.h"
#include "cache_system/cache_system_types.h"
#include "object_storage/object_storage_types.h"

#include <chaos/common/utility/StartableService.h>

namespace chaos{
    namespace metadata_service {
        struct setting {
            //!define the availability zone that own the metadata service instance(the default zone is "default")
            std::string                         ha_zone_name;
            //!identify the number of the sandbox to use in the batch system
            unsigned int                        batch_sandbox_size;
            unsigned int                        cron_job_scheduler_repeat_time;
            unsigned int                        cron_job_ageing_management_repeat_time;
            std::string							persistence_implementation;
            std::vector<std::string>			persistence_server_list;
            std::map<std::string, std::string>	persistence_kv_param_map;
            
            //!cache configuration
            chaos::metadata_service::cache_system::CacheDriverSetting cache_driver_setting;
            
            //----------object storage configuration----------------
            chaos::metadata_service::object_storage::ObjStorageSetting  object_storage_setting;
            
            chaos::metadata_service::worker::DataWorkerSetting          worker_setting;
        };
        
        struct ApiSubserviceAccessor {
            //! network broker for talk with others chaos node
            chaos::common::network::NetworkBroker *network_broker_service;
            
            //! batch executor engine
            chaos::common::utility::StartableServiceContainer<batch::MDSBatchExecutor> batch_executor;
        };
        
    }
}
#endif
