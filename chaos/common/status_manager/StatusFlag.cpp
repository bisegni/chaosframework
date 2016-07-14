/*
 *	status_manager_types.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 11/07/16 INFN, National Institute of Nuclear Physics
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

#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/status_manager/StatusFlag.h>

using namespace chaos::common::utility;
using namespace chaos::common::status_manager;

#pragma mark StateLevel
StateLevel::StateLevel():
value(0),
description(),
severity(StatusFlagServerityOperationl),
occurence(0){}

StateLevel::StateLevel(const int8_t _value,
                       const std::string& _description,
                       StatusFlagServerity _severity):
value(_value),
description(_description),
severity(_severity),
occurence(0){}

StateLevel::StateLevel(const StateLevel& src):
value(src.value),
description(src.description),
severity(src.severity),
occurence(src.occurence){}

bool StateLevel::operator< (const StateLevel &right) {
    return value < right.value;
}

#pragma mark StatusFlagListener
StatusFlagListener::StatusFlagListener():
listener_uuid(UUIDUtil::generateUUIDLite()){}

StatusFlagListener::~StatusFlagListener(){}

const std::string& StatusFlagListener::getStatusFlagListenerUUID(){
    return listener_uuid;
}

#pragma mark StatusFlag
StatusFlag::StatusFlag(const std::string& _name,
                       const std::string& _description):
flag_uuid(UUIDUtil::generateUUIDLite()),
name(_name),
description(_description),
current_level(0){}

StatusFlag::StatusFlag(const StatusFlag& src):
flag_uuid(UUIDUtil::generateUUIDLite()),
name(src.name),
description(src.description),
current_level(src.current_level){}

bool StatusFlag::addLevel(const StateLevel& level_state) {
    StatusLevelContainerOrderedIndex& ordered_index = boost::multi_index::get<ordered_index_tag>(set_levels);
    //chec if the level has been already added
    if(ordered_index.find(level_state.value) != ordered_index.end()) return false;
    //add the level state description
    set_levels.insert(level_state);
    return true;
}

//! add a new level with level state
bool StatusFlag::addLevelsFromSet(const StateLevelContainer& src_set_levels) {
    StatusLevelContainerOrderedIndex& local_ordered_index = set_levels.get<ordered_index_tag>();
    const StatusLevelContainerOrderedIndex& src_ordered_index = src_set_levels.get<ordered_index_tag>();
    for(StatusLevelContainerOrderedIndexConstIterator it = src_ordered_index.begin(),
        end = src_ordered_index.end();
        it != end;
        it++){
        //check if key is already present
        if(local_ordered_index.find(it->value) != local_ordered_index.end()) continue;
        
        //add level
        set_levels.insert(*it);
    }
    
    return true;
}

void StatusFlag::setCurrentLevel(int8_t _current_level) {
    StateLevelContainerIncrementCounter increment_counter;
    StatusLevelContainerOrderedIndex& local_ordered_index = set_levels.get<ordered_index_tag>();
    StatusLevelContainerOrderedIndexIterator it = local_ordered_index.find(_current_level);
    if(it == local_ordered_index.end()) return;
    if(current_level != _current_level){
        current_level = _current_level;
        fireToListener();
    } else {
        local_ordered_index.modify(it, increment_counter);
    }
}

int8_t StatusFlag::getCurrentLevel() const {
    return current_level;
}

const StateLevel& StatusFlag::getCurrentStateLevel() {
    StatusLevelContainerOrderedIndex& local_ordered_index = set_levels.get<ordered_index_tag>();
    StatusLevelContainerOrderedIndexIterator it = local_ordered_index.find(getCurrentLevel());
    return *it;
}

void StatusFlag::addListener(StatusFlagListener *new_listener) {
    boost::shared_lock<boost::shared_mutex> wl(mutex_listener);
    listener.insert(new_listener);
}

void StatusFlag::removeListener(StatusFlagListener *erase_listener) {
    listener.erase(erase_listener);
}

void StatusFlag::fireToListener() {
    for(SetListnerIterator it = listener.begin(),
        end = listener.end();
        it != end;
        it++){
        (*it)->statusFlagUpdated(flag_uuid);
    }
}
#pragma mark StatusFlagBoolState
StatusFlagBoolState::StatusFlagBoolState(const std::string& _name,
                                         const std::string& _description):
StatusFlag(_name,
           _description){
    addLevel(StateLevel(0, "Off"));
    addLevel(StateLevel(1, "On"));
}

StatusFlagBoolState::StatusFlagBoolState(const StatusFlagBoolState& src):
StatusFlag(src){}

bool StatusFlagBoolState::addLevel(const StateLevel& level_state) {
    return StatusFlag::addLevel(level_state);
}

void StatusFlagBoolState::setState(bool state){
    setCurrentLevel((int)state);
}
