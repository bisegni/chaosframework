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

#include <chaos/common/state_flag/StateFlag.h>
#include <chaos/common/utility/AbstractListenerContainer.h>

#include <boost/shared_ptr.hpp>

namespace chaos{
    namespace common {
        namespace alarm {
            //!forward decalration
            class AlarmCatalog;
            class AlarmDescription;
            
            //!Alarm handler abstraction
            class AlarmHandler:
            public chaos::common::state_flag::StateFlagListener {
                friend class AlarmDescription;
                void stateFlagUpdated(const std::string& flag_uuid,
                                      const std::string& flag_name,
                                      const chaos::common::state_flag::StateFlagServerity level_severity);
            protected:
                virtual void alarmChanged(const std::string& alarm_name,
                                          const int8_t alarm_severity) = 0;
            };
            
            //!base alarm description
            class AlarmDescription:
            protected state_flag::StateFlag {
                friend class AlarmCatalog;
            public:
                AlarmDescription(const std::string alarm_name,
                                 const std::string alarm_description);
                ~AlarmDescription();
            protected:;
                bool addState(int8_t severity_code,
                              const std::string& severity_tag,
                              const std::string& severity_description,
                              const chaos::common::state_flag::StateFlagServerity severity);
                
            public:
                const std::string& getAlarmName() const;
                const std::string& getAlarmDescription() const;
                void setCurrentSeverity(const uint8_t);
                const int8_t getCurrentSeverityCode() const;
                const std::string& getCurrentSeverityTag() const;
                const std::string& getCurrentSeverityDescription() const;
            };
            
            typedef boost::shared_ptr<AlarmDescription> AlarmDescriptionShrdPtr;
            
        }
    }
}

#endif /* __CHAOSFramework_C540A441_6F28_4F99_9489_7331538962BD_AlarmDescription_h */
