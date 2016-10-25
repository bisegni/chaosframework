/*
 *	alarm_types.h
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

#ifndef __CHAOSFramework__D8800D8_A68A_4897_84EA_4751DBE25779_alarm_types_h
#define __CHAOSFramework__D8800D8_A68A_4897_84EA_4751DBE25779_alarm_types_h

#include <string>
#include <stdint.h>

namespace chaos {
    namespace common {
        namespace alarm {
            
            //!the possible level of an alert
            typedef enum AlarmLevel {
                AlarmLevelClear = 0,//green
                AlarmLevelLow,//blue
                AlarmLevelElevated,//yellow
                AlarmLevelHigh,//orange
                AlarmLevelSevere//red
            } LogLevel;
            
            //!alert information about occurence and push behaviour
            struct AlarmStat {
                //!sign the last timestamp(in milliseconds) that the log code has been fired
                uint64_t next_fire_ts;
                //!is the delay in millisecond that the code can be processed
                uint64_t delay_for_next_message;
                //!sign the repetition
                uint32_t repetition;
                
                AlarmStat();
                ~AlarmStat();
            };
            
            //! keep the statisti on how much an error code is used
            struct AlarmDescription {
                //!alert code
                const short alarm_code;
                //!alert code
                const std::string alarm_name;
                //!alert description
                const std::string alarm_description;
                //!curren talarm level
                AlarmLevel current_level;
                //!alert statistic information
                AlarmStat stat;
                
                AlarmDescription();
                ~AlarmDescription();
                static std::string decodeAlarmLevelDescription(const AlarmDescription& alarm_reference);
            };
        }
    }
}

#endif /* __CHAOSFramework__D8800D8_A68A_4897_84EA_4751DBE25779_alarm_types_h */
