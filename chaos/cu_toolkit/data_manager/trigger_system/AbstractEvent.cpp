/*
 *	AbstractEventType.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 29/07/16 INFN, National Institute of Nuclear Physics
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

#include <chaos/cu_toolkit/data_manager/trigger_system/AbstractEvent.h>

#include <chaos/common/utility/UUIDUtil.h>

using namespace chaos::common::utility;
using namespace chaos::cu::data_manager::trigger_system;

#pragma mark AbstractEvent
AbstractEvent::AbstractEvent(const std::string& _event_name,
                             const std::string& _event_description,
                             const unsigned int _event_code):
event_name(_event_name),
event_description(_event_description),
event_code(_event_code){}

const std::string& AbstractEvent::getEventName() const {
    return event_name;
}
const std::string& AbstractEvent::getEventDescription() const {
    return event_description;
}
const unsigned int& AbstractEvent::getEventCode() const {
    return event_code;
}

#pragma mark MultiIndex Key Extractor
const AbstractEventMIExstractName::result_type&
AbstractEventMIExstractName::operator()(const AbstractEventShrdPtr &p) const {
    return p->event_name;
}

const AbstractEventMIExstractCode::result_type&
AbstractEventMIExstractCode::operator()(const AbstractEventShrdPtr &p) const {
    return p->event_code;
}
