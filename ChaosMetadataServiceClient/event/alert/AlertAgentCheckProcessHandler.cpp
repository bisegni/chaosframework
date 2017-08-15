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

#include <ChaosMetadataServiceClient/event/alert/AlertAgentCheckProcessHandler.h>

using namespace chaos::common::event;
using namespace chaos::common::event::alert;

using namespace chaos::metadata_service_client::event::alert;

AlertAgentCheckProcessHandler::AlertAgentCheckProcessHandler():
AlertEventHandler(EventAlertAgentCheckProcessSubmitted) {
    
}

AlertAgentCheckProcessHandler::~AlertAgentCheckProcessHandler() {
    
}

void AlertAgentCheckProcessHandler::handleEvent(const EventDescriptor * const event) {
    AlertEventDescriptor *alert_event_descirptor = (AlertEventDescriptor*)event;
    std::string agent_uid;
    int32_t check_result;
    alert_event_descirptor->getAgentCheckProcessAlert(agent_uid,
                                                      check_result);
    
    handleAgentEvent(agent_uid,
                     check_result);
}
