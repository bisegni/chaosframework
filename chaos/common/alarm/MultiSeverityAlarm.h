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

#ifndef __CHAOSFramework_A4507F2C_C3B0_42C8_BD1B_CB0D18C71724_MultiSeverityAlarm_h
#define __CHAOSFramework_A4507F2C_C3B0_42C8_BD1B_CB0D18C71724_MultiSeverityAlarm_h

#include <chaos/common/alarm/AlarmDescription.h>

namespace chaos{
    namespace common {
        namespace alarm {
            
            typedef enum MultiSeverityAlarmLevel {
                MultiSeverityAlarmLevelClear,
                MultiSeverityAlarmLevelWarning,
                MultiSeverityAlarmLevelHigh
            } MultiSeverityAlarmLevel;
            
            //!Alarm handler abstraction
            class MultiSeverityAlarmHandler:
            public chaos::common::state_flag::StateFlagListener {
                friend class AlarmDescription;;
                void alarmChanged(const std::string& alarm_tag,
                                  const std::string& alarm_name,
                                  const int8_t alarm_severity);
            protected:
                virtual void alarmChanged(const std::string& alarm_tag,
                                          const std::string& alarm_name,
                                          const MultiSeverityAlarmLevel alarm_severity) = 0;
                
            };
            
            class MultiSeverityAlarm:
            public AlarmDescription {
                
            public:
                MultiSeverityAlarm(const std::string& alarm_tag,
                                   const std::string& alarm_name,
                                   const std::string& alarm_description);
                ~MultiSeverityAlarm();
                
                void setNewSeverity(const MultiSeverityAlarmLevel new_severity);
                
                const MultiSeverityAlarmLevel getCurrentSeverityLevel();
            };
            
        }
    }
}

#endif /* __CHAOSFramework_A4507F2C_C3B0_42C8_BD1B_CB0D18C71724_MultiSeverityAlarm_h */
