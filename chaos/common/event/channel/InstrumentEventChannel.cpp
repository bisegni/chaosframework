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

#include <chaos/common/global.h>
#include <chaos/common/event/channel/InstrumentEventChannel.h>
#include <chaos/common/event/evt_desc/InstrumentEventDescriptor.h>

using namespace chaos;
using namespace chaos::common::network;
using namespace chaos::common::event;
using namespace chaos::common::event::channel;
using namespace chaos::common::event::instrument;

//-----------------------------------------------------
InstrumentEventChannel::InstrumentEventChannel(NetworkBroker *rootBroker):EventChannel(rootBroker) {
    
}

//-----------------------------------------------------
InstrumentEventChannel::~InstrumentEventChannel() {
    
}

void InstrumentEventChannel::handleEvent(const chaos::common::event::EventDescriptor * const event) {
    LAPP_ << "InstrumentEventChannel::handleEvent";
}

//--------------------inherited-----------------
void InstrumentEventChannel::activateChannelEventReception(EventAction *event_action) {
    //activate the reception for the event type alert
    EventChannel::_activateChannelEventReception(event_action,
                                                 EventTypeAlert);
}

//-----------------------------------------------------
int InstrumentEventChannel::sendEvent(const std::string& identification,
                                      uint16_t sub_code,
                                      uint16_t priority,
                                      EventDataType type_of_data,
                                      const void *value_ptr,
                                      uint16_t value_size) {
    instrument::InstrumentEventDescriptor *ied = new instrument::InstrumentEventDescriptor();
    ied->setInstrument(identification,
                       sub_code,
                       priority,
                       type_of_data,
                       value_ptr,
                       value_size);
    return EventChannel::sendRawEvent(ied);
}

//-----------------------------------------------------
int InstrumentEventChannel::notifyForScheduleUpdateWithNewValue(const char * insturmentID, uint64_t newScheduleUpdateTime) {
    instrument::InstrumentEventDescriptor *ied = new instrument::InstrumentEventDescriptor();
    ied->setNewScheduleDelay(insturmentID, newScheduleUpdateTime);
    return EventChannel::sendRawEvent(ied);
}

//-----------------------------------------------------
int InstrumentEventChannel::notifyForAttributeSetting(const char * insturmentID, uint16_t error) {
    instrument::InstrumentEventDescriptor *ied = new instrument::InstrumentEventDescriptor();
    ied->setDatasetInputAttributeChanged(insturmentID, error);
    return EventChannel::sendRawEvent(ied);
}
//! Send an event for the heartbeat
/*!
 \param insturmentID the indetificaiton of instrument
 */
int InstrumentEventChannel::notifyHeartbeat(const char * insturmentID) {
    instrument::InstrumentEventDescriptor *ied = new instrument::InstrumentEventDescriptor();
    ied->setEartbeat(insturmentID);
    return EventChannel::sendRawEvent(ied);
}