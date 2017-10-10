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

#ifndef __CHAOSFramework__9C99B80_FDA6_4B63_AA62_F784EF4786E5_MDSCronusManager_h
#define __CHAOSFramework__9C99B80_FDA6_4B63_AA62_F784EF4786E5_MDSCronusManager_h

#include <chaos/common/cronus_manager/CronusManager.h>

#include <chaos/common/utility/Singleton.h>

#include "MDSCronJob.h"

namespace chaos {
    namespace metadata_service {
        class ChaosMetadataService;
        
        namespace cron_job {
            
            class MDSCronusManager:
            public chaos::common::cronus_manager::CronusManager,
            public chaos::common::utility::Singleton<MDSCronusManager> {
                friend class chaos::common::utility::Singleton<MDSCronusManager>;
                friend class chaos::metadata_service::ChaosMetadataService;
            private:
                MDSCronusManager();
                ~MDSCronusManager();
                chaos::service_common::persistence::data_access::AbstractPersistenceDriver *abstract_persistance_driver;
            public:
                void init(void *init_data) throw(chaos::CException);
                void deinit() throw(chaos::CException);
                bool addJob(MDSCronJob *new_job,
                            std::string& job_index,
                            uint64_t repeat_delay,
                            uint64_t offset = 0);
            };
        }
    }
}

#endif /* __CHAOSFramework__9C99B80_FDA6_4B63_AA62_F784EF4786E5_MDSCronusManager_h */
