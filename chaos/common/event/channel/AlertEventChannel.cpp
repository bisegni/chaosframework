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
#include <string>
#include <chaos/common/global.h>
#include <chaos/common/event/channel/AlertEventChannel.h>

using namespace chaos;
using namespace chaos::common::event;
using namespace chaos::common::network;
using namespace chaos::common::event::channel;

//-----------------------------------------------------
AlertEventChannel::AlertEventChannel(NetworkBroker *rootBroker):EventChannel(rootBroker) {
    
}

//-----------------------------------------------------
AlertEventChannel::~AlertEventChannel() {
    
}

void AlertEventChannel::handleEvent(const EventDescriptor * const event) {
    LAPP_ << "AlertEventChannel::handleEvent";
}

//--------------------inherited-----------------
void AlertEventChannel::activateChannelEventReception(EventAction *event_action) {
    //activate the reception for the event type alert
    EventChannel::_activateChannelEventReception(event_action,
                                                 EventTypeAlert);
}
//-----------------------------------------------------
int AlertEventChannel::sendEvent(const std::string& identification,
                                 uint16_t sub_code,
                                 uint16_t priority,
                                 EventDataType type_of_data,
                                 const void *value_ptr,
                                 uint16_t value_size) {
    alert::AlertEventDescriptor *aed = new alert::AlertEventDescriptor();
    aed->setAlert(identification,
                  sub_code,
                  priority,
                  type_of_data,
                  value_ptr,
                  value_size);
    return EventChannel::sendRawEvent(aed);
}

int AlertEventChannel::sendLogAlert(const std::string& node_uid,
                                    const std::string& log_domain) {
    alert::AlertEventDescriptor *aed = new alert::AlertEventDescriptor();
    aed->setLogAlert(node_uid,
                     log_domain);
    return EventChannel::sendRawEvent(aed);
}

int AlertEventChannel::sendAgentProcessCheckAlert(const std::string& agent_uid,
                                                  const int32_t check_result) {
    alert::AlertEventDescriptor *aed = new alert::AlertEventDescriptor();
    aed->setAgentCheckProcessAlert(agent_uid,
                                   check_result);
    return EventChannel::sendRawEvent(aed);
}
