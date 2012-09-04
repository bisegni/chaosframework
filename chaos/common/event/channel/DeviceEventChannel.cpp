/*
 *	DeviceEventChannel.cpp
 *	CHAOSFramework
 *	Created by Claudio Bisegni on 02/09/12.
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
#include <chaos/common/event/channel/DeviceEventChannel.h>
#include <chaos/common/event/evt_desc/InstrumentEventDescriptor.h>

using namespace chaos;
using namespace chaos::event;
using namespace chaos::event::channel;
using namespace chaos::event::instrument;

    //-----------------------------------------------------
DeviceEventChannel::DeviceEventChannel(MessageBroker *rootBroker):EventChannel(rootBroker) {
    
}

    //-----------------------------------------------------
DeviceEventChannel::~DeviceEventChannel() {
    
}

void DeviceEventChannel::handleEvent(const event::EventDescriptor * const event) {
    LAPP_ << "DeviceEventChannel::handleEvent";
}

    //--------------------inherited-----------------
void DeviceEventChannel::activateChannelEventReception() {
        //activate the reception for the event type alert
    EventChannel::activateChannelEventReception(EventTypeAlert);
}

    //-----------------------------------------------------
int DeviceEventChannel::sendEvent(const char * const identificationString, uint16_t subCode, uint16_t priority, EventDataType typeOfData, const void *valuePtr, uint16_t valueSize) {
    instrument::InstrumentEventDescriptor *ied = new instrument::InstrumentEventDescriptor();
    ied->setInstrument(identificationString, strlen(identificationString), subCode, priority, typeOfData, valuePtr, valueSize);
    return EventChannel::sendRawEvent(ied);
}

    //-----------------------------------------------------
int DeviceEventChannel::notifyForScheduleUpdateWithNewValue(const char * deviceID, uint64_t newScheduleUpdateTime) {
    instrument::InstrumentEventDescriptor *ied = new instrument::InstrumentEventDescriptor();
    ied->setNewScheduleDelay(deviceID, newScheduleUpdateTime);
    return EventChannel::sendRawEvent(ied);
}