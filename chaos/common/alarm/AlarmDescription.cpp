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
using namespace chaos::common::status_manager;

AlarmDescription::AlarmDescription(const std::string alarm_name,
                                   const std::string alarm_description):
StatusFlag(alarm_name,
           alarm_description){}

AlarmDescription::~AlarmDescription() {}

bool AlarmDescription::addState(int8_t severity_code,
                                const std::string& severity_tag,
                                const std::string& severity_description,
                                const StatusFlagServerity severity) {
    return StatusFlag::addLevel(StateLevel(severity_code,
                                           severity_tag,
                                           severity_description,
                                           severity));
}

const std::string& AlarmDescription::getAlarmName() const {
    return StatusFlag::getName();
}
const std::string& AlarmDescription::getAlarmDescription() const {
    return StatusFlag::getDescription();
}

void AlarmDescription::setCurrentSeverity(const uint8_t new_severity_code) {
    StatusFlag::setCurrentLevel(new_severity_code);
}

const int8_t AlarmDescription::getCurrentSeverityCode() const {
    return StatusFlag::getCurrentStateLevel().getValue();
}

const std::string& AlarmDescription::getCurrentSeverityTag() const {
    return StatusFlag::getCurrentStateLevel().getTag();
}

const std::string& AlarmDescription::getCurrentSeverityDescription() const {
    return StatusFlag::getCurrentStateLevel().getDescription();
}
