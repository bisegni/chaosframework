/*
 *	MultiSeverityAlarm.cpp
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

#include <chaos/common/alarm/MultiSeverityAlarm.h>

using namespace chaos::common::alarm;
using namespace chaos::common::state_flag;

#pragma MultiSeverityAlarmHandler
void MultiSeverityAlarmHandler::alarmChanged(const std::string& alarm_name,
                                             const int8_t alarm_severity) {
    alarmChanged(alarm_name,
                 static_cast<MultiSeverityAlarmLevel>(alarm_severity));
}


#pragma MultiSeverityAlarm
MultiSeverityAlarm::MultiSeverityAlarm(const std::string& alarm_name,
                                       const std::string& alarm_description):

AlarmDescription(alarm_name,
                 alarm_description){
    addState(MultiSeverityAlarmLevelClear,
             "clear",
             "Alarm is in a regular state",
             StateFlagServerityRegular);
    addState(MultiSeverityAlarmLevelWarning,
             "Warning",
             "Low probability that something will fails, attention is needed",
             StateFlagServerityWarning);
    addState(MultiSeverityAlarmLevelHigh,
             "High",
             "High probability that something is going to fails",
             StateFlagServerityHigh);
    
}

MultiSeverityAlarm::~MultiSeverityAlarm() {}

void MultiSeverityAlarm::setNewSeverity(const MultiSeverityAlarmLevel new_severity) {
    setCurrentLevel(new_severity);
}

const MultiSeverityAlarmLevel MultiSeverityAlarm::getCurrentSeverityLevel() {
    return static_cast<MultiSeverityAlarmLevel>(getCurrentLevel());
}
