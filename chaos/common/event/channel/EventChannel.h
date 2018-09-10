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

#ifndef __CHAOSFramework__EventChannel__
#define __CHAOSFramework__EventChannel__

#include <string>
#include <chaos/common/event/evt_desc/EventDescriptor.h>
#include <chaos/common/action/EventAction.h>
#include <chaos/common/exception/exception.h>

namespace chaos {
    
    //forward declaration
    namespace common {
        namespace network {
            class NetworkBroker;
        }
        
        namespace event {
            namespace channel {
                
                //! Managment for send and receive event
                /*!
                 The Event Channel permit the forward of a general event descriptor. Every channel
                 can be registerd for receive event from other node.
                 */
                class EventChannel {
                    
                    // make the broker friendly of this class to access private and protected memeber
                    friend class chaos::common::network::NetworkBroker;
                    
                    // channel identification ID
                    std::string channelID;
                    
                    //broker for event forwarding
                    chaos::common::network::NetworkBroker *messageBroker;
                protected:
                    
                    EventChannel(chaos::common::network::NetworkBroker *rootBroker);
                    
                    virtual ~EventChannel();
                    
                    void init() ;
                    
                    void deinit() ;
                    
                    int sendRawEvent(EventDescriptor *newEvent);
                    
                    //!
                    /*!
                     */
                    virtual int sendEvent(const std::string& identificationString,
                                          uint16_t subCode,
                                          uint16_t priority,
                                          EventDataType typeOfData,
                                          const void *valuePtr,
                                          uint16_t valueSize= 0) = 0;
                    
                    
                    void _activateChannelEventReception(EventAction *event_action,
                                                       EventType eventType);
                    
                public:
                    
                    virtual void activateChannelEventReception(EventAction *event_action) = 0;
                    
                    void deactivateChannelEventReception(EventAction *event_action);
                    
                    //!
                    /*!
                     */
                    int sendEventInt8(const char * const identificationString, uint16_t subCode, uint16_t priority, uint8_t value);
                    
                    //!
                    /*!
                     */
                    int sendEventInt16(const char * const identificationString, uint16_t subCode, uint16_t priority, uint16_t value);
                    
                    //!
                    /*!
                     */
                    int sendEventInt32(const char * const identificationString, uint16_t subCode, uint16_t priority, uint32_t value);
                    
                    //!
                    /*!
                     */
                    int sendEventInt64(const char * const identificationString, uint16_t subCode, uint16_t priority, uint64_t value);
                    
                    //!
                    /*!
                     */
                    int sendEventDouble(const char * const identificationString, uint16_t subCode, uint16_t priority, double value);
                    
                    //!
                    /*!
                     */
                    int sendEventCString(const char * const identificationString, uint16_t subCode, uint16_t priority, const char * value);
                    
                    //!
                    /*!
                     */
                    int sendEventBinary(const char * const identificationString, uint16_t subCode, uint16_t priority, void * value, uint16_t length);
                };
            }
        }
    }
}
#endif /* defined(__CHAOSFramework__EventChannel__) */
