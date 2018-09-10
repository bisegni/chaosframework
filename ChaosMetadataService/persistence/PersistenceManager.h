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
                void init(void* init_data) ;
                void deinit() ;
                
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
