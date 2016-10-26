/*
 *	AlarmDescription.h
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

#ifndef __CHAOSFramework_C540A441_6F28_4F99_9489_7331538962BD_AlarmDescription_h
#define __CHAOSFramework_C540A441_6F28_4F99_9489_7331538962BD_AlarmDescription_h

#include <chaos/common/status_manager/StatusFlag.h>

namespace chaos{
    namespace common {
        namespace alarm {
            
            class AlarmDescription:
            protected status_manager::StatusFlag {
            public:
                AlarmDescription(const std::string alarm_name,
                                 const std::string alarm_description);
                ~AlarmDescription();
            protected:
                bool addState(int8_t severity_code,
                                 const std::string& severity_tag,
                                 const std::string& severity_description,
                                 const chaos::common::status_manager::StatusFlagServerity severity);
                
            public:
                const std::string& getAlarmName() const;
                const std::string& getAlarmDescription() const;
                void setCurrentSeverity(const uint8_t);
                const int8_t getCurrentSeverityCode() const;
                const std::string& getCurrentSeverityTag() const;
                const std::string& getCurrentSeverityDescription() const;
            };
            
        }
    }
}

#endif /* __CHAOSFramework_C540A441_6F28_4F99_9489_7331538962BD_AlarmDescription_h */
