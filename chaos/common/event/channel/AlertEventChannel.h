/*
 *	AlertEventChannel.h
 *	CHAOSFramework
 *	Created by Claudio Bisegni on 28/08/12.
 *
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__AlertEventChannel__
#define __CHAOSFramework__AlertEventChannel__

#include <chaos/common/event/channel/EventChannel.h>
#include <chaos/common/event/evt_desc/AlertEventDescriptor.h>

#include <string>

namespace chaos {
    //! forward declarataion
    namespace common {
        namespace network {
            class NetworkBroker;
        }
        
        namespace event {
            namespace channel {
                //! Channel for manage the alert event type
                /*!
                 This channel simplify the alert event forwarding and recivement
                 */
                class AlertEventChannel :
                public EventChannel {
                    friend class chaos::common::network::NetworkBroker;
                    
                protected:
                    
                    AlertEventChannel(chaos::common::network::NetworkBroker *rootBroker);
                    
                    ~AlertEventChannel();
                    
                    void handleEvent(const event::EventDescriptor * const event);
                    
                    int sendEvent(const std::string& identification,
                                  uint16_t sub_code,
                                  uint16_t priority,
                                  EventDataType type_of_data,
                                  const void *value_ptr,
                                  uint16_t value_size = 0);
                public:
                    
                    //--------------------inherited-----------------
                    void activateChannelEventReception(EventAction *event_action);
                    
                    int sendLogAlert(const std::string& node_uid,
                                     const std::string& log_domain);
                    
                    int sendAgentProcessCheckAlert(const std::string& agent_uid,
                                                   const int32_t check_result);
                };
                
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__AlertEventChannel__) */
