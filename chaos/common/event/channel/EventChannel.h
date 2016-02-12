/*
 *	EventChannel.h
 *	CHAOSFramework
 *	Created by Claudio Bisegni on 27/08/12.
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
                class EventChannel:
                public EventAction {
                    
                    // make the broker friendly of this class to access private and protected memeber
                    friend class chaos::common::network::NetworkBroker;
                    
                    // channel identification ID
                    std::string channelID;
                    
                    //broker for event forwarding
                    chaos::common::network::NetworkBroker *messageBroker;
                    
                protected:
                    
                    EventChannel(chaos::common::network::NetworkBroker *rootBroker);
                    
                    virtual ~EventChannel();
                    
                    void init() throw (CException);
                    
                    void deinit() throw (CException);
                    
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
                    
                    //-------------------inherited--------------------
                    virtual void handleEvent(const event::EventDescriptor * const event) = 0;
                    
                    
                    void activateChannelEventReception(EventType eventType);
                    
                public:
                    
                    virtual void activateChannelEventReception() = 0;
                    
                    void deactivateChannelEventReception();
                    
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
