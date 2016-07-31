/*
 *	EventConsumerDatasetChange.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 29/07/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__A5E9B423_6474_43C8_92B3_A4FCA8AC8C77_RegisterEventConsumerEnvironment_h
#define __CHAOSFramework__A5E9B423_6474_43C8_92B3_A4FCA8AC8C77_RegisterEventConsumerEnvironment_h

#include <chaos/cu_toolkit//data_manager/trigger_system/AbstractEvent.h>
#include <chaos/cu_toolkit//data_manager/trigger_system/AbstractConsumer.h>

namespace chaos {
    namespace cu {
        namespace data_manager {
            namespace trigger_system {
                
                template<typename BEvent,
                         typename BEConsumer,
                         typename BETarget,
                         typename EventType>
                class RegisterEventConsumerEnvironment {
                    
                    void registerEvent(BEvent *);
                };

            }
        }
    }
}

#endif /* RegisterEventCOnsumerEnvironment_hpp */
