/*
 *	AlertEventChannel.cpp
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

#include <chaos/common/global.h>
#include <chaos/common/event/channel/AlertEventChannel.h>

using namespace chaos;
using namespace chaos::event;
using namespace chaos::event::channel;

    //-----------------------------------------------------
AlertEventChannel::AlertEventChannel(MessageBroker *rootBroker):EventChannel(rootBroker) {
    
}

    //-----------------------------------------------------
AlertEventChannel::~AlertEventChannel() {
    
}

void AlertEventChannel::handleEvent(event::EventDescriptor *event) {
    
}

    //--------------------inherited-----------------
void AlertEventChannel::activateChannelEventReception() {
        //activate the reception for the event type alert
    EventChannel::activateChannelEventReception(EventTypeAlert);
}

    //-----------------------------------------------------
int AlertEventChannel::sendAlert(alert::EventAlertCode alertCode, uint16_t priority, EventDataType typeOfData, const void *valuePtr, uint16_t valueSize) {
    alert::AlertEventDescriptor *aed = new alert::AlertEventDescriptor();
    aed->initData();
    aed->setAlert(alertCode, priority, typeOfData, valuePtr, valueSize);
    return sendRawEvent(aed);
}

    //-----------------------------------------------------
int AlertEventChannel::sendAlertInt8(alert::EventAlertCode alertCode, uint16_t priority, uint8_t value){
    return sendAlert(alertCode, priority, EventDataInt8, &value);
}

    //-----------------------------------------------------
int AlertEventChannel::sendAlertInt16(alert::EventAlertCode alertCode, uint16_t priority, uint16_t value){
    return sendAlert(alertCode, priority, EventDataInt16, &value);
}

    //-----------------------------------------------------
int AlertEventChannel::sendAlertInt32(alert::EventAlertCode alertCode, uint16_t priority, uint32_t value){
    return sendAlert(alertCode, priority, EventDataInt32, &value);

}

    //-----------------------------------------------------
int AlertEventChannel::sendAlertInt64(alert::EventAlertCode alertCode, uint16_t priority, uint64_t value){
    return sendAlert(alertCode, priority, EventDataInt64, &value);
}

    //-----------------------------------------------------
int AlertEventChannel::sendAlertDouble(alert::EventAlertCode alertCode, uint16_t priority, double value){
    return sendAlert(alertCode, priority, EventDataDouble, &value);
}

    //-----------------------------------------------------
int AlertEventChannel::sendAlertCString(alert::EventAlertCode alertCode, uint16_t priority, const char * value) {
    return sendAlert(alertCode, priority, EventDataDouble, value, strlen(value));
}

    //-----------------------------------------------------
int AlertEventChannel::sendAlertBinary(alert::EventAlertCode alertCode, uint16_t priority, void * value, uint16_t length) {
    return sendAlert(alertCode, priority, EventDataDouble, value, length);
}