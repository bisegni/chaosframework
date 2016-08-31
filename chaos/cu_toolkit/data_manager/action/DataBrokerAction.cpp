/*
 *	DataBrokerAction.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 31/08/16 INFN, National Institute of Nuclear Physics
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

#include <chaos/cu_toolkit/data_manager/action/DataBrokerAction.h>

using namespace chaos::common::utility;
using namespace chaos::cu::data_manager::action;

DataBrokerAction::DataBrokerAction(const std::string& action_name):
NamedService(action_name){}

DataBrokerAction::~DataBrokerAction() {}

void DataBrokerAction::setManagers(manipulation::DataBrokerEditor *_data_broker_editor,
                                   publishing::PublishingManager *_publishing_manager,
                                   trigger_system::EventManager *_event_manager) {
    data_broker_editor =  _data_broker_editor;
    publishing_manager =  _publishing_manager;
    event_manager = _event_manager;
}
