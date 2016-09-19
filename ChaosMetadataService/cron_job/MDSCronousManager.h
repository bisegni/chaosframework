/*
 *	MDSCronousManager.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 19/09/2016 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__9C99B80_FDA6_4B63_AA62_F784EF4786E5_MDSCronousManager_h
#define __CHAOSFramework__9C99B80_FDA6_4B63_AA62_F784EF4786E5_MDSCronousManager_h

#include <chaos/common/cronous_manager/CronousManager.h>

#include <chaos/common/utility/Singleton.h>

#include "MDSCronJob.h"

namespace chaos {
    namespace metadata_service {
        class ChaosMetadataService;
        
        namespace cron_job {
            
            class MDSCronousManager:
            public chaos::common::cronous_manager::CronousManager,
            public chaos::common::utility::Singleton<MDSCronousManager> {
                friend class chaos::common::utility::Singleton<MDSCronousManager>;
                friend class chaos::metadata_service::ChaosMetadataService;
            private:
                MDSCronousManager();
                ~MDSCronousManager();
                chaos::service_common::persistence::data_access::AbstractPersistenceDriver *abstract_persistance_driver;
            public:
                bool addJob(MDSCronJob *new_job,
                            std::string& job_index,
                            uint64_t repeat_delay,
                            uint64_t offset = 0);
            };
        }
    }
}

#endif /* __CHAOSFramework__9C99B80_FDA6_4B63_AA62_F784EF4786E5_MDSCronousManager_h */
