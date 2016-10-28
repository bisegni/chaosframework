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

#include <chaos/common/alarm/AlarmCatalog.h>

using namespace chaos::common::data;
using namespace chaos::common::alarm;
using namespace chaos::common::state_flag;

AlarmCatalog::AlarmCatalog(const std::string& catalog_name):
StateFlagCatalog(catalog_name){}

AlarmCatalog::~AlarmCatalog() {}

void AlarmCatalog::addAlarm(AlarmDescription *new_alarm) {
    StateFlag *state_flag_ptr = dynamic_cast<StateFlag*>(new_alarm);
    StateFlagCatalog::addFlag(boost::shared_ptr<StateFlag>(state_flag_ptr));
}

AlarmDescription *AlarmCatalog::getAlarmByName(const std::string& alarm_name) {
    boost::shared_ptr<StateFlag> alarm = StateFlagCatalog::getFlagByName(alarm_name);
    return static_cast<AlarmDescription*>(alarm.get());
}

AlarmDescription *AlarmCatalog::getAlarmByOrderedID(const unsigned int alarm_ordered_id) {
    boost::shared_ptr<StateFlag> alarm = StateFlagCatalog::getFlagByOrderedID(alarm_ordered_id);
    return static_cast<AlarmDescription*>(alarm.get());
}

std::auto_ptr<chaos::common::data::CDataBuffer> AlarmCatalog::getRawFlagsLevel() {
    return StateFlagCatalog::getRawFlagsLevel();
}

void AlarmCatalog::setApplyRawFlagsValue(std::auto_ptr<chaos::common::data::CDataBuffer>& raw_level) {
    StateFlagCatalog::setApplyRawFlagsValue(raw_level);
}

std::auto_ptr<CDataWrapper> AlarmCatalog::serialize() {
    StateFlagCatalogSDWrapper sd_wrap(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(StateFlagCatalog, *this));
    return sd_wrap.serialize();
}

void AlarmCatalog::deserialize(CDataWrapper *serialized_data) {
    StateFlagCatalogSDWrapper sd_wrap(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(StateFlagCatalog, *this));
    return sd_wrap.deserialize(serialized_data);
}
