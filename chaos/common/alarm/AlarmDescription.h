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
                void stateFlagUpdated(const chaos::common::state_flag::FlagDescription     flag_description,
                                      const std::string&                                    level_name,
                                      const chaos::common::state_flag::StateFlagServerity   current_level_severity);
            protected:
                virtual void alarmChanged(const std::string& alarm_tag,
                                          const std::string& alarm_name,
                                          const int8_t alarm_severity) = 0;
            };
            
            //!base alarm description
            class AlarmDescription:
            protected state_flag::StateFlag {
                friend class AlarmCatalog;
                uint64_t lastAlarmChanged_ms;
            
            public:
                AlarmDescription(const std::string& alarm_tag,
                                 const std::string& alarm_name,
                                 const std::string& alarm_description);
                ~AlarmDescription();
                
            public:
                const std::string& getAlarmTag() const;
                const std::string& getAlarmName() const;
                const std::string& getAlarmDescription() const;
                void setCurrentSeverity(const uint8_t);
                const int8_t getCurrentSeverityCode() const;
                const std::string& getCurrentSeverityTag() const;
                const std::string& getCurrentSeverityDescription() const;
                uint64_t getLastUpdateTimestamp();
                bool addState(int8_t severity_code,
                              const std::string& severity_tag,
                              const std::string& severity_description,
                              const chaos::common::state_flag::StateFlagServerity severity);
            };
            
            typedef ChaosSharedPtr<AlarmDescription> AlarmDescriptionShrdPtr;
            
        }
    }
}

#endif /* __CHAOSFramework_C540A441_6F28_4F99_9489_7331538962BD_AlarmDescription_h */
