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

#ifndef __CHAOSFramework_C21113B7_23E7_40DF_8C69_818E440F920C_MDSCronJob_h
#define __CHAOSFramework_C21113B7_23E7_40DF_8C69_818E440F920C_MDSCronJob_h

#include "../persistence/persistence.h"

#include <chaos/common/global.h>
#include <chaos/common/cronus_manager/CronJob.h>


namespace chaos {
    namespace metadata_service {
        namespace cron_job {
            
            class MDSCronusManager;
            
            class MDSCronJob:
            public chaos::common::cronus_manager::CronJob {
                friend class MDSCronusManager;
                //dataaccess abstract driver
                chaos::service_common::persistence::data_access::AbstractPersistenceDriver& abstract_persistance_driver;
            protected:
                template<typename T>
                T* getDataAccess() {
                    return abstract_persistance_driver.getDataAccess<T>();
                }
            public:
                MDSCronJob(chaos::common::data::CDataWrapper *param);
                ~MDSCronJob();
            };
            
        }
    }
}

#endif /* __CHAOSFramework_C21113B7_23E7_40DF_8C69_818E440F920C_MDSCronJob_h */
