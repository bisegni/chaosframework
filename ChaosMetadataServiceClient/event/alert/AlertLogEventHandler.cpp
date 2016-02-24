/*
 *	AlertLogEventHandler.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 16/02/16 INFN, National Institute of Nuclear Physics
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

#include <ChaosMetadataServiceClient/event/alert/AlertLogEventHandler.h>

using namespace chaos::common::event;
using namespace chaos::common::event::alert;

using namespace chaos::metadata_service_client::event::alert;

AlertLogEventHandler::AlertLogEventHandler():
AlertEventHandler(EventAlertLogSubmitted) {
    
}

AlertLogEventHandler::~AlertLogEventHandler() {
    
}

void AlertLogEventHandler::handleEvent(const EventDescriptor * const event) {
    AlertEventDescriptor *alert_event_descirptor = (AlertEventDescriptor*)event;
    std::string node_uid;
    std::string log_domian;
    alert_event_descirptor->getLogAlert(node_uid,log_domian);

    handleLogEvent(node_uid,
                   log_domian);
}