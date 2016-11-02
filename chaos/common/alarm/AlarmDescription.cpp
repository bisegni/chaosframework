/*
 *	AlarmDescription.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 26/10/2016 INFN, National Institute of Nuclear Physics
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

#include <chaos/common/alarm/AlarmDescription.h>

using namespace chaos::common::alarm;
using namespace chaos::common::utility;
using namespace chaos::common::state_flag;

#pragma mark AlarmHandler
void AlarmHandler::stateFlagUpdated(const std::string& flag_uuid,
                                    const std::string& flag_name,
                                    const StateFlagServerity level_severity) {
    alarmChanged(flag_name,
                 level_severity);
}

#pragma mark AlarmDescription
AlarmDescription::AlarmDescription(const std::string alarm_name,
                                   const std::string alarm_description):
StateFlag(alarm_name,
          alarm_description){}

AlarmDescription::~AlarmDescription() {}

bool AlarmDescription::addState(int8_t severity_code,
                                const std::string& severity_tag,
                                const std::string& severity_description,
                                const StateFlagServerity severity) {
    return StateFlag::addLevel(StateLevel(severity_code,
                                          severity_tag,
                                          severity_description,
                                          severity));
}

const std::string& AlarmDescription::getAlarmName() const {
    return StateFlag::getName();
}
const std::string& AlarmDescription::getAlarmDescription() const {
    return StateFlag::getDescription();
}

void AlarmDescription::setCurrentSeverity(const uint8_t new_severity_code) {
    StateFlag::setCurrentLevel(new_severity_code);
}

const int8_t AlarmDescription::getCurrentSeverityCode() const {
    return StateFlag::getCurrentStateLevel().getValue();
}

const std::string& AlarmDescription::getCurrentSeverityTag() const {
    return StateFlag::getCurrentStateLevel().getTag();
}

const std::string& AlarmDescription::getCurrentSeverityDescription() const {
    return StateFlag::getCurrentStateLevel().getDescription();
}


