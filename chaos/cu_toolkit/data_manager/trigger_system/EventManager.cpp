/*
 *	EventManager.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 28/07/16 INFN, National Institute of Nuclear Physics
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

#include <chaos/cu_toolkit/data_manager/trigger_system/EventManager.h>

using namespace chaos::cu::data_manager::trigger_system;

EventManager::EventManager():
index_event_code(event_container.get<TEMITagCode>()),
index_event_name(event_container.get<TEMITagName>()){
    
}

EventManager::~EventManager() {
    
}

void EventManager::addEvent(TriggerEventShrdPtr trigger_event) {
    boost::unique_lock<boost::shared_mutex> wl(mutex_event_container);
    event_container.insert(trigger_event);
}
