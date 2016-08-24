/*
 *	EventManager.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 28/07/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_C1346138_0E86_4813_9DE2_2C2B525C6C1C_EventManager_h
#define __CHAOSFramework_C1346138_0E86_4813_9DE2_2C2B525C6C1C_EventManager_h

#include <chaos/common/utility/LockableObject.h>

#include <chaos/cu_toolkit/data_manager/data_manager_types.h>
#include <chaos/cu_toolkit/data_manager/trigger_system/dataset_event/dataset_event.h>

#include <boost/thread.hpp>

namespace chaos {
    namespace cu {
        namespace data_manager {
            namespace trigger_system {

                //!this is the main class where all other sublayer fire event with data
                /*!
                 Event dispatcher collect even fired by other sublayer and attach it to algorithm execution
                 */
                class EventManager {
                    //! reference to master dataset container
                    chaos::common::utility::LockableObject<DatasetElementContainer>& container_dataset;
                    
                    //!trigger environment
                    chaos::cu::data_manager::trigger_system::dataset_event::DatasetRegisterEnviroment dataset_trigger_environment;
                public:
                    EventManager(chaos::common::utility::LockableObject<DatasetElementContainer>& _container_dataset);
                    ~EventManager();

                    //!scan all adataset and fetch all attribute to register it as subject
                    int registerDatasetAttributeAsSubject();
                };
                
            }
        }
    }
}

#endif /* __CHAOSFramework_C1346138_0E86_4813_9DE2_2C2B525C6C1C_EventManager_h */
