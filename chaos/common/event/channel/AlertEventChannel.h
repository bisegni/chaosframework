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
