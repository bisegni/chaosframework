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