/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */

#include <chaos/common/alarm/AlarmDescription.h>

using namespace chaos::common::alarm;
using namespace chaos::common::utility;
using namespace chaos::common::state_flag;

#pragma mark AlarmHandler
void AlarmHandler::stateFlagUpdated(const FlagDescription       flag_description,
                                    const std::string&          level_name,
                                    const StateFlagServerity    current_level_severity) {
    alarmChanged(flag_description.tag,
                 flag_description.name,
                 current_level_severity);
}

#pragma mark AlarmDescription
AlarmDescription::AlarmDescription(const std::string& alarm_tag,
                                   const std::string& alarm_name,
                                   const std::string& alarm_description):
StateFlag(alarm_name,
          alarm_description){setTag(alarm_tag);}

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

const std::string& AlarmDescription::getAlarmTag() const {
    return StateFlag::getTag();
}
