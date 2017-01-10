/*
 *	state_flag_types.cpp
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

#include <chaos/common/global.h>
#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/state_flag/StateFlag.h>

using namespace chaos::common::utility;
using namespace chaos::common::state_flag;

#define SL_LOG INFO_LOG(StateFlag)
#define SL_DBG DBG_LOG(StateFlag)
#define SL_ERR ERR_LOG(StateFlag)

FlagDescription::FlagDescription(const std::string& _name,
                                 const std::string& _description):
tag(),
uuid(UUIDUtil::generateUUIDLite()),
name(_name),
description(_description) {}

FlagDescription::FlagDescription(const FlagDescription& _description):
tag(_description.tag),
uuid(_description.uuid),
name(_description.name),
description(_description.description){}

FlagDescription::~FlagDescription() {}

#pragma mark StateLevel
StateLevel::StateLevel():
value(0),
description(),
severity(StateFlagServerityRegular),
occurence(0){}

StateLevel::StateLevel(const int8_t _value,
                       const std::string& _tag,
                       const std::string& _description,
                       StateFlagServerity _severity):
value(_value),
tag(_tag),
description(_description),
severity(_severity),
occurence(0){}

StateLevel::StateLevel(const StateLevel& src):
value(src.value),
tag(src.tag),
description(src.description),
severity(src.severity),
occurence(src.occurence){}

bool StateLevel::operator< (const StateLevel &right) {
    return value < right.value;
}

StateLevel& StateLevel::operator=(StateLevel const &rhs) {
    value = rhs.value;
    tag = rhs.tag;
    description = rhs.description;
    severity = rhs.severity;
    occurence = rhs.occurence;
    return *this;
}

const std::string& StateLevel::getTag() const {
    return tag;
}

const std::string& StateLevel::getDescription() const {
    return description;
}

const int8_t StateLevel::getValue() const {
    return value;
}
const StateFlagServerity StateLevel::getSeverity() const {
    return severity;
}

#pragma mark StateFlagListener
StateFlagListener::StateFlagListener():
listener_uuid(UUIDUtil::generateUUIDLite()){}

StateFlagListener::~StateFlagListener(){}

const std::string& StateFlagListener::getStateFlagListenerUUID(){
    return listener_uuid;
}

#pragma mark StateFlag
StateFlag::StateFlag():
flag_description("",""),
current_level(0){}


StateFlag::StateFlag(const std::string& _name,
                     const std::string& _description):
flag_description(_name,
                 _description),
current_level(0){}

StateFlag::StateFlag(const StateFlag& src):
flag_description(src.flag_description),
current_level(src.current_level){}

StateFlag& StateFlag::operator=(StateFlag const &rhs) {
    flag_description.name = rhs.flag_description.name;
    flag_description.description = rhs.flag_description.description;
    current_level = rhs.current_level;
    set_levels = rhs.set_levels;
    return *this;
};

const std::string& StateFlag::getDescription() const{
    return flag_description.description;
}

const std::string& StateFlag::getFlagUUID() const {
    return flag_description.uuid;
}

const std::string& StateFlag::getName() const {
    return flag_description.name;
}

bool StateFlag::addLevel(const StateLevel& level_state) {
    StatusLevelContainerOrderedIndex& ordered_index = boost::multi_index::get<ordered_index_tag>(set_levels);
    //chec if the level has been already added
    if(ordered_index.find(level_state.getValue()) != ordered_index.end()) return false;
    //add the level state description
    set_levels.insert(level_state);
    if(set_levels.size() == 1) {
        //set this state as current selected
        current_level = level_state.getValue();
    }
    DEBUG_CODE(SL_DBG << CHAOS_FORMAT("Code: %1%, tag:%2%, Desc:%3%, Severity:%4%", %(int32_t)level_state.getValue()%level_state.getTag()%level_state.getDescription()%level_state.getSeverity());)
    return true;
}

//! add a new level with level state
bool StateFlag::addLevelsFromSet(const StateLevelContainer& src_set_levels) {
    StatusLevelContainerOrderedIndex& local_ordered_index = set_levels.get<ordered_index_tag>();
    const StatusLevelContainerOrderedIndex& src_ordered_index = src_set_levels.get<ordered_index_tag>();
    for(StatusLevelContainerOrderedIndexConstIterator it = src_ordered_index.begin(),
        end = src_ordered_index.end();
        it != end;
        it++){
        //check if key is already present
        if(local_ordered_index.find(it->getValue()) != local_ordered_index.end()) continue;
        
        //add level
        set_levels.insert(*it);
        DEBUG_CODE(SL_DBG << CHAOS_FORMAT("Code: %1%, tag:%2%, Desc:%3%, Severity:%4%", %(int32_t)it->getValue()%it->getTag()%it->getDescription()%it->getSeverity());)
    }
    return true;
}

void StateFlag::setCurrentLevel(int8_t _current_level) {
    StateLevelContainerIncrementCounter increment_counter;
    StatusLevelContainerOrderedIndex& local_ordered_index = set_levels.get<ordered_index_tag>();
    StatusLevelContainerOrderedIndexIterator it = local_ordered_index.find(_current_level);
    if(it == local_ordered_index.end()) return;
    if(current_level != _current_level){
        current_level = _current_level;
        fire(0);
    } else {
        local_ordered_index.modify(it, increment_counter);
    }
}

int8_t StateFlag::getCurrentLevel() const {
    return current_level;
}

const StateLevel& StateFlag::getCurrentStateLevel() const {
    StatusLevelContainerOrderedIndex& local_ordered_index = set_levels.get<ordered_index_tag>();
    StatusLevelContainerOrderedIndexIterator it = local_ordered_index.find(getCurrentLevel());
    return *it;
}

void StateFlag::fireToListener(unsigned int fire_code,
                               chaos::common::utility::AbstractListener *listener_to_fire) {
    StateFlagListener *state_flag_listener_instance = dynamic_cast<StateFlagListener*>(listener_to_fire);
    if(state_flag_listener_instance){
        const StateLevel& level = getCurrentStateLevel();
        state_flag_listener_instance->stateFlagUpdated(flag_description,
                                                       level.getTag(),
                                                       level.getSeverity());
    }
}

void StateFlag::setTag(const std::string& new_tag) {
    flag_description.tag = new_tag;
}

const std::string& StateFlag::getTag() {
    return flag_description.tag;
}

#pragma mark StateFlagBoolState
StateFlagBoolState::StateFlagBoolState(const std::string& _name,
                                       const std::string& _description,
                                       const StateLevel& off_state_level,
                                       const StateLevel& on_state_level):
StateFlag(_name,
          _description){
    addLevel(off_state_level);
    addLevel(on_state_level);
}

StateFlagBoolState::StateFlagBoolState(const StateFlagBoolState& src):
StateFlag(src){}

bool StateFlagBoolState::addLevel(const StateLevel& level_state) {
    return StateFlag::addLevel(level_state);
}

void StateFlagBoolState::setState(bool state){
    setCurrentLevel((int)state);
}
