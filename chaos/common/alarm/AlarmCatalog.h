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
                AlarmCatalog(const std::string& catalog_name);
                ~AlarmCatalog();
                
                //! add a new alarm to catalog
                /*!
                 Alarm ownership will be taken by catalog and managed by it
                 */
                void addAlarm(AlarmDescription *new_alarm);
                
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
                
                std::auto_ptr<chaos::common::data::CDataBuffer> getRawFlagsLevel();
                
                void setApplyRawFlagsValue(std::auto_ptr<chaos::common::data::CDataBuffer>& raw_level);
                
                std::auto_ptr<chaos::common::data::CDataWrapper> serialize();
                
                void deserialize(chaos::common::data::CDataWrapper *serialized_data);
            };
        }
    }
}

#endif /* __CHAOSFramework__0CB64FD_9567_4577_BF46_5C47DBEE7998_AlarmCatalog_h */
