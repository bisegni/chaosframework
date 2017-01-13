/*
 *	AlarmCatalog.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 27/10/2016 INFN, National Institute of Nuclear Physics
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
                std::auto_ptr<chaos::common::data::CDataBuffer> getRawFlagsLevel();
                
                //!Set the raw alarm state of alarm current state
                /*!
                 all byte describe the current state of an alarm so the rtrsulting
                 memory as size = num of alarm * char)
                 */
                void setApplyRawFlagsValue(std::auto_ptr<chaos::common::data::CDataBuffer>& raw_level);
                
                //!return the serialization of description of alarm catalog
                std::auto_ptr<chaos::common::data::CDataWrapper> serialize();
                
                //!deserialize the description of a previously serialized catalog
                void deserialize(chaos::common::data::CDataWrapper *serialized_data);
                
                //!return true if all alarm are in a regular state
                const bool isCatalogClear();
                
                //!return the catalog size
                const size_t size();

                //!return the catalog size
                 const uint8_t max();
            };
        }
    }
}

#endif /* __CHAOSFramework__0CB64FD_9567_4577_BF46_5C47DBEE7998_AlarmCatalog_h */
