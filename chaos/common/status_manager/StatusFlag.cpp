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
description(),
severity(StatusFlagServerityOperationl),
occurence(0){}

StateLevel::StateLevel(const std::string& _description,
                       StatusFlagServerity _severity):
description(_description),
severity(_severity),
occurence(0){}

StateLevel::StateLevel(const StateLevel& src):
description(src.description),
severity(src.severity),
occurence(src.occurence){}

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

bool StatusFlag::addLevel(int8_t level, const StateLevel& level_state) {
    //chec if the level has been already added
    if(map_level_tag.find(level) != map_level_tag.end()) return false;
    //add the level state description
    map_level_tag.insert(MapFlagStateLevelPair(level, level_state));
    return true;
}

//! add a new level with level state
bool StatusFlag::addLevelsFromMap(const MapFlagStateLevel& src_levels_map) {
    for(MapFlagStateLevelConstIterator it = src_levels_map.begin(),
        end = src_levels_map.end();
        it != end;
        it++){
        //check if key is already present
        if(map_level_tag.count(it->first)) continue;
        
        //add level
        map_level_tag.insert(MapFlagStateLevelPair(it->first, it->second));
    }
    
    return true;
}

void StatusFlag::setCurrentLevel(int8_t _current_level) {
    if(map_level_tag.find(_current_level) != map_level_tag.end()) return;
    if(current_level != _current_level){
        current_level = _current_level;
        map_level_tag[current_level].occurence = 0;
        if(listener){listener->statusFlagUpdated(flag_uuid);}
    } else {
        map_level_tag[current_level].occurence++;
    }
}

int8_t StatusFlag::getCurrentLevel() const {
    return current_level;
}

const StateLevel& StatusFlag::getCurrentStateLevel() {
    return map_level_tag[getCurrentLevel()];
}

const std::string& StatusFlag::getFlagUUID() {
    return flag_uuid;
}

void StatusFlag::setListener(StatusFlagListener *new_listener) {
    boost::shared_lock<boost::shared_mutex> wl(mutex_listener);
    listener = new_listener;
}

#pragma mark StatusFlagBoolState
StatusFlagBoolState::StatusFlagBoolState(const std::string& _name,
                                         const std::string& _description):
StatusFlag(_name,
           _description){
    addLevel(0, StateLevel("Off"));
    addLevel(1, StateLevel("On"));
}

StatusFlagBoolState::StatusFlagBoolState(const StatusFlagBoolState& src):
StatusFlag(src){}

bool StatusFlagBoolState::addLevel(int8_t level, const StateLevel& level_state) {
    return StatusFlag::addLevel(level, level_state);
}

void StatusFlagBoolState::setState(bool state){
    setCurrentLevel((int)state);
}
