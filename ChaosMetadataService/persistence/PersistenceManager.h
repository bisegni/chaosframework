/*
 *	PersistenceManager.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 14/12/2016 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__69F68F6_B60D_4DB6_AAE7_353F17F6B75E_PeristenceManager_h
#define __CHAOSFramework__69F68F6_B60D_4DB6_AAE7_353F17F6B75E_PeristenceManager_h

#include <chaos/common/utility/Singleton.h>
#include <chaos/common/utility/InizializableService.h>
#include <chaos_service_common/persistence/data_access/AbstractPersistenceDriver.h>

namespace chaos {
    namespace metadata_service{
        namespace persistence {
            
            class PersistenceManager:
            public chaos::common::utility::InizializableService,
            public chaos::common::utility::Singleton<PersistenceManager> {
                common::utility::InizializableServiceContainer<chaos::service_common::persistence::data_access::AbstractPersistenceDriver> persistence_driver;
            public:
                PersistenceManager();
                ~PersistenceManager();
                void init(void* init_data) throw (chaos::CException);
                void deinit() throw (chaos::CException);
                
                template<typename T>
                T* getDataAccess() {
                    if(persistence_driver.get() == NULL) throw CException(-1, "No Driver Found", __PRETTY_FUNCTION__);
                    return persistence_driver->getDataAccess<T>();
                }
            };
        }
    }
}
//! persistence driver instance
//;

#endif /* __CHAOSFramework__69F68F6_B60D_4DB6_AAE7_353F17F6B75E_PeristenceManager_h */
