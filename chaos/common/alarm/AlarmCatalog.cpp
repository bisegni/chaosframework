/*
 *	AlarmCatalog.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 27/10/2016 INFN, National Institute of Nuclear Physics
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
#include <chaos/common/alarm/AlarmCatalog.h>

using namespace chaos::common::data;
using namespace chaos::common::alarm;
using namespace chaos::common::state_flag;

AlarmCatalog::AlarmCatalog():
StateFlagCatalog("AlarmCatalog"){}

AlarmCatalog::AlarmCatalog(const std::string& catalog_name):
StateFlagCatalog(catalog_name){}

AlarmCatalog::AlarmCatalog(const AlarmCatalog& catalog):
StateFlagCatalog(catalog) {}

AlarmCatalog::~AlarmCatalog() {}

void AlarmCatalog::addAlarm(AlarmDescription *new_alarm) {
    CHAOS_ASSERT(new_alarm);
    StateFlag *state_flag_ptr = dynamic_cast<StateFlag*>(new_alarm);
    StateFlagCatalog::addFlag(ChaosSharedPtr<StateFlag>(state_flag_ptr));
}

bool AlarmCatalog::addAlarmHandler(const std::string& alarm_name,
                     AlarmHandler *alarm_handler) {
    CHAOS_ASSERT(alarm_handler);
    AlarmDescription *alarm = getAlarmByName(alarm_name);
    if(alarm == NULL) return false;
    alarm->addListener(alarm_handler);
    return true;
}

bool AlarmCatalog::removeAlarmHandler(const std::string& alarm_name,
                        AlarmHandler *alarm_handler) {
    CHAOS_ASSERT(alarm_handler);
    AlarmDescription *alarm = getAlarmByName(alarm_name);
    if(alarm == NULL) return false;
    alarm->removeListener(alarm_handler);
    return true;
}

AlarmDescription *AlarmCatalog::getAlarmByName(const std::string& alarm_name) {
    ChaosSharedPtr<StateFlag> alarm = StateFlagCatalog::getFlagByName(alarm_name);
    return static_cast<AlarmDescription*>(alarm.get());
}

AlarmDescription *AlarmCatalog::getAlarmByOrderedID(const unsigned int alarm_ordered_id) {
    ChaosSharedPtr<StateFlag> alarm = StateFlagCatalog::getFlagByOrderedID(alarm_ordered_id);
    return static_cast<AlarmDescription*>(alarm.get());
}

void AlarmCatalog::setAllAlarmSeverity(int8_t new_severity) {
    StateFlagCatalog::setAllFlagState(new_severity);
}

ChaosUniquePtr<chaos::common::data::CDataBuffer> AlarmCatalog::getRawFlagsLevel() {
    return StateFlagCatalog::getRawFlagsLevel();
}

void AlarmCatalog::setApplyRawFlagsValue(ChaosUniquePtr<chaos::common::data::CDataBuffer>& raw_level) {
    StateFlagCatalog::setApplyRawFlagsValue(raw_level);
}

ChaosUniquePtr<CDataWrapper> AlarmCatalog::serialize() {
    StateFlagCatalogSDWrapper sd_wrap(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(StateFlagCatalog, *this));
    return sd_wrap.serialize();
}

void AlarmCatalog::deserialize(CDataWrapper *serialized_data) {
    StateFlagCatalogSDWrapper sd_wrap(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(StateFlagCatalog, *this));
    return sd_wrap.deserialize(serialized_data);
}

const bool AlarmCatalog::isCatalogClear() {
    VectorStateFlag sf_list;
    getFlagsForSeverity(StateFlagServerityRegular, sf_list);
    //if flag in regular state are equals to the total dimension of catalog
    // ti mean that are no flag are in some severity state
    return StateFlagCatalog::size() == sf_list.size();
}

const size_t AlarmCatalog::size() {
    return StateFlagCatalog::size();
}
const uint8_t AlarmCatalog::max() {
    return StateFlagCatalog::max();
}
