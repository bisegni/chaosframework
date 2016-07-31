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

#include <chaos/cu_toolkit/data_manager/trigger_system/AbstractEvent.h>

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
                    boost::shared_mutex     mutex_event_container;
                    AbstractEventContainer   event_container;
                    
                    TECodeIndex& index_event_code;
                    TENameIndex& index_event_name;
                public:
                    EventManager();
                    ~EventManager();
                    
                    void addEvent(AbstractEventShrdPtr trigger_event);
                };
                
            }
        }
    }
}

#endif /* __CHAOSFramework_C1346138_0E86_4813_9DE2_2C2B525C6C1C_EventManager_h */
