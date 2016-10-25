/*
 *	alarm_types.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 25/10/2016 INFN, National Institute of Nuclear Physics
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

#include <chaos/common/alarm/alarm_types.h>

#define ALARM_CLEAR_DESCRIPTION     "Clear"
#define ALARM_LOW_DESCRIPTION       "Low"
#define ALARM_ELEVATED_DESCRIPTION  "Elevated"
#define ALARM_HIGH_DESCRIPTION      "High"
#define ALARM_SEVERE_DESCRIPTION    "Severe"

using namespace chaos::common::alarm;

#pragma AlarmStat
AlarmStat::AlarmStat():
next_fire_ts(0),
delay_for_next_message(0),
repetition(0){}

AlarmStat::~AlarmStat() {}

#pragma AlarmDescription
AlarmDescription::AlarmDescription():
alarm_code(0),
alarm_name(),
alarm_description(),
current_level(AlarmLevelClear){}

AlarmDescription::~AlarmDescription() {}


std::string AlarmDescription::decodeAlarmLevelDescription(const AlarmDescription& alarm_reference) {
    std::string description;
    switch (alarm_reference.alarm_code) {
        case AlarmLevelClear:
            description = ALARM_CLEAR_DESCRIPTION;
            break;
        case AlarmLevelLow:
            description = ALARM_LOW_DESCRIPTION;
            break;
        case AlarmLevelElevated:
            description = ALARM_ELEVATED_DESCRIPTION;
            break;
        case AlarmLevelHigh:
            description = ALARM_HIGH_DESCRIPTION;
            break;
        case AlarmLevelSevere:
            description = ALARM_SEVERE_DESCRIPTION;
            break;
    }
    return description;
}
