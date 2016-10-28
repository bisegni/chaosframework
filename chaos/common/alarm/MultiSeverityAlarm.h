/*
 *	MultiSeverityAlarm.h
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

#ifndef __CHAOSFramework_A4507F2C_C3B0_42C8_BD1B_CB0D18C71724_MultiSeverityAlarm_h
#define __CHAOSFramework_A4507F2C_C3B0_42C8_BD1B_CB0D18C71724_MultiSeverityAlarm_h

#include <chaos/common/alarm/AlarmDescription.h>

namespace chaos{
    namespace common {
        namespace alarm {
            
            typedef enum MultiSeverityAlarmLevel {
                MultiSeverityAlarmLevelClear,
                MultiSeverityAlarmLevelLow,
                MultiSeverityAlarmLevelHig
            } MultiSeverityAlarmLevel;
            
            //!Alarm handler abstraction
            class MultiSeverityAlarmHandler:
            public chaos::common::state_flag::StateFlagListener {
                friend class AlarmDescription;;
                void alarmChanged(const std::string& alarm_name,
                                  const int8_t alarm_severity);
            protected:
                virtual void alarmChanged(const std::string& alarm_name,
                                          const MultiSeverityAlarmLevel alarm_severity) = 0;
                
            };
            
            class MultiSeverityAlarm:
            protected AlarmDescription {
            public:
                MultiSeverityAlarm(const std::string alarm_name,
                                   const std::string alarm_description);
                ~MultiSeverityAlarm();
                
                void setNewSeverity(const MultiSeverityAlarmLevel new_severity);
                
                const MultiSeverityAlarmLevel getCurrentSeverityLevel();
            };
            
        }
    }
}

#endif /* __CHAOSFramework_A4507F2C_C3B0_42C8_BD1B_CB0D18C71724_MultiSeverityAlarm_h */
