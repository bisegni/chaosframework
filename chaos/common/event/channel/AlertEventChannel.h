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
namespace chaos {
    
    class MessageBroker;
    
    namespace event {
        namespace channel {
            
                //! Channel for manage the alert event type
            /*!
             This channel simplify the alert event forwarding and recivement
             */
            class AlertEventChannel : public EventChannel {
                friend class chaos::MessageBroker;
                
                    //!
                /*!
                 */
                inline int sendAlert(alert::EventAlertCode alertCode, uint16_t priority, EventDataType typeOfData, const void *valuePtr, uint16_t valueSize = 0);
         
            protected:
                
                AlertEventChannel(MessageBroker *rootBroker);
                
                ~AlertEventChannel();
                void handleEvent(event::EventDescriptor *event);
            public:
                
                    //--------------------inherited-----------------
                void activateChannelEventReception();
                    //!
                /*!
                 */
                int sendAlertInt8(alert::EventAlertCode alertCode, uint16_t priority, uint8_t value);
                
                    //!
                /*!
                 */
                int sendAlertInt16(alert::EventAlertCode alertCode, uint16_t priority, uint16_t value);
                
                    //!
                /*!
                 */
                int sendAlertInt32(alert::EventAlertCode alertCode, uint16_t priority, uint32_t value);
                
                    //!
                /*!
                 */
                int sendAlertInt64(alert::EventAlertCode alertCode, uint16_t priority, uint64_t value);
                
                    //!
                /*!
                 */
                int sendAlertDouble(alert::EventAlertCode alertCode, uint16_t priority, double value);
                
                    //!
                /*!
                 */
                int sendAlertCString(alert::EventAlertCode alertCode, uint16_t priority, const char * value);
                
                    //!
                /*!
                 */
                int sendAlertBinary(alert::EventAlertCode alertCode, uint16_t priority, void * value, uint16_t length);
            };
            
        }
    }
}

#endif /* defined(__CHAOSFramework__AlertEventChannel__) */
