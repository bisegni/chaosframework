/*
 *	EventChannel.cpp
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

#include <chaos/common/event/channel/EventChannel.h>
#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/utility/UUIDUtil.h>

using namespace std;
using namespace chaos;
using namespace chaos::common::utility;
using namespace chaos::common::event;
using namespace chaos::common::event::channel;

EventChannel::EventChannel(NetworkBroker *_messageBroker):
messageBroker(_messageBroker),
channelID(UUIDUtil::generateUUIDLite()) {
}

EventChannel::~EventChannel() {
    
}

void EventChannel::init() throw (CException) {
    
}

void EventChannel::deinit() throw (CException) {
}

void EventChannel::_activateChannelEventReception(EventAction *event_action,
                                                 EventType eventType) {
    CHAOS_ASSERT(messageBroker && event_action)
    messageBroker->registerEventAction(event_action, eventType);
    
}

void EventChannel::deactivateChannelEventReception(EventAction *event_action) {
    CHAOS_ASSERT(messageBroker && event_action)
    messageBroker->deregisterEventAction(event_action);
}

int EventChannel::sendRawEvent(EventDescriptor *newEvent) {
    CHAOS_ASSERT(messageBroker)
    return messageBroker->submitEvent(newEvent);
}

    //-----------------------------------------------------
int EventChannel::sendEventInt8(const char * const identificationString, uint16_t subCode, uint16_t priority, uint8_t value){
    return sendEvent(identificationString, subCode, priority, EventDataInt8, &value);
}

    //-----------------------------------------------------
int EventChannel::sendEventInt16(const char * const identificationString, uint16_t subCode, uint16_t priority, uint16_t value){
    return sendEvent(identificationString, subCode, priority, EventDataInt16, &value);
}

    //-----------------------------------------------------
int EventChannel::sendEventInt32(const char * const identificationString, uint16_t subCode, uint16_t priority, uint32_t value){
    return sendEvent(identificationString, subCode, priority, EventDataInt32, &value);
    
}

    //-----------------------------------------------------
int EventChannel::sendEventInt64(const char * const identificationString, uint16_t subCode, uint16_t priority, uint64_t value){
    return sendEvent(identificationString, subCode, priority, EventDataInt64, &value);
}

    //-----------------------------------------------------
int EventChannel::sendEventDouble(const char * const identificationString, uint16_t subCode, uint16_t priority, double value){
    return sendEvent(identificationString, subCode, priority, EventDataDouble, &value);
}

    //-----------------------------------------------------
int EventChannel::sendEventCString(const char * const identificationString, uint16_t subCode, uint16_t priority, const char * value) {
    return sendEvent(identificationString, subCode, priority, EventDataDouble, value, strlen(value));
}

    //-----------------------------------------------------
int EventChannel::sendEventBinary(const char * const identificationString, uint16_t subCode, uint16_t priority, void * value, uint16_t length) {
    return sendEvent(identificationString, subCode, priority, EventDataDouble, value, length);
}