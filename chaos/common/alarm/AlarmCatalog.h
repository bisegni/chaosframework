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

#ifndef __CHAOSFramework__0CB64FD_9567_4577_BF46_5C47DBEE7998_AlarmCatalog_h
#define __CHAOSFramework__0CB64FD_9567_4577_BF46_5C47DBEE7998_AlarmCatalog_h

#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/alarm/AlarmDescription.h>
#include <chaos/common/state_flag/StateFlagCatalog.h>
#include <chaos/common/state_flag/StateFlagCatalogSDWrapper.h>

namespace chaos {
    namespace common {
        namespace alarm {
            
            class AlarmCatalog:
            protected chaos::common::state_flag::StateFlagCatalog {
            public:
                AlarmCatalog();
                AlarmCatalog(const std::string& catalog_name);
                AlarmCatalog(const AlarmCatalog& catalog);
                ~AlarmCatalog();
                
                //! add a new alarm to catalog
                /*!
                 Alarm ownership will be taken by catalog and managed by it
                 */
                void addAlarm(AlarmDescription *new_alarm);
                
                bool addAlarmHandler(const std::string& alarm_name,
                                     AlarmHandler *alarm_handler);
                
                bool removeAlarmHandler(const std::string& alarm_name,
                                        AlarmHandler *alarm_handler);
                
                //!return a pointer to the alarm description
                /*!
                 The ownerhip of the instance will not be passed to the caller
                 */
                AlarmDescription *getAlarmByName(const std::string& alarm_name);
                
                //!return a point to alarm pointer to the ordered id
                /*!
                 Ordered id is equivalend to the insertion order. The ownerhip of 
                 the instance will not be passed to the caller
                 */
                AlarmDescription *getAlarmByOrderedID(const unsigned int alarm_ordered_id);
                
                //!set the severity on all alarm
                void setAllAlarmSeverity(int8_t new_severity);
                
                //!Return the raw alarm current state
                /*!
                 all byte describe the current state of an alarm so the rtrsulting
                 memory as size = num of alarm * char)
                 */
                ChaosUniquePtr<chaos::common::data::CDataBuffer> getRawFlagsLevel();
                
                //!Set the raw alarm state of alarm current state
                /*!
                 all byte describe the current state of an alarm so the rtrsulting
                 memory as size = num of alarm * char)
                 */
                void setApplyRawFlagsValue(ChaosUniquePtr<chaos::common::data::CDataBuffer>& raw_level);
                
                //!return the serialization of description of alarm catalog
                ChaosUniquePtr<chaos::common::data::CDataWrapper> serialize();
                
                //!deserialize the description of a previously serialized catalog
                void deserialize(chaos::common::data::CDataWrapper *serialized_data);
                
                //!return true if all alarm are in a regular state
                const bool isCatalogClear();
                
                //!return the catalog size
                const size_t size();

                //!return the catalog size
                 const uint8_t maxLevel();
            };
        }
    }
}

#endif /* __CHAOSFramework__0CB64FD_9567_4577_BF46_5C47DBEE7998_AlarmCatalog_h */
