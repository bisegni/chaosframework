/*
 *	MDSCronJob.h
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

#ifndef __CHAOSFramework_C21113B7_23E7_40DF_8C69_818E440F920C_MDSCronJob_h
#define __CHAOSFramework_C21113B7_23E7_40DF_8C69_818E440F920C_MDSCronJob_h

#include <chaos/common/cronous_manager/CronJob.h>
#include <chaos_service_common/persistence/data_access/AbstractPersistenceDriver.h>

namespace chaos {
    namespace metadata_service {
        namespace cron_job {
            
            class MDSCronousManager;
            
            class MDSCronJob:
            public chaos::common::cronous_manager::CronJob {
                friend class MDSCronousManager;
                //dataaccess abstract driver
                chaos::service_common::persistence::data_access::AbstractPersistenceDriver *abstract_persistance_driver;
            protected:
                template<typename T>
                T* getDataAccess() {
                    CHAOS_ASSERT(abstract_persistance_driver)
                    return abstract_persistance_driver->getDataAccess<T>();
                }
            };
            
        }
    }
}

#endif /* __CHAOSFramework_C21113B7_23E7_40DF_8C69_818E440F920C_MDSCronJob_h */
