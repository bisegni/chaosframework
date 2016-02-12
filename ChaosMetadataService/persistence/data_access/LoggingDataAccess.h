/*
 *	LoggingDataAccess.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 11/02/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__LoggingDataAccess_h
#define __CHAOSFramework__LoggingDataAccess_h


#include "../persistence.h"

#include <chaos/common/chaos_types.h>

namespace chaos {
    namespace metadata_service {
        namespace persistence {
            namespace data_access {
                
                CHAOS_DEFINE_MAP_FOR_TYPE(std::string, std::string, LoggingKeyValueStringMap)
                CHAOS_DEFINE_MAP_FOR_TYPE(std::string, int64_t,     LoggingKeyValueInt64Map)
                CHAOS_DEFINE_MAP_FOR_TYPE(std::string, int32_t,     LoggingKeyValueInt32Map)
                CHAOS_DEFINE_MAP_FOR_TYPE(std::string, double,      LoggingKeyValueDoubleMap)
                CHAOS_DEFINE_MAP_FOR_TYPE(std::string, bool,        LoggingKeyValueBoolMap)
                
                //! define and entry of the log
                struct LogEntry {
                    uint64_t                    sequence;
                    uint64_t                    ts;
                    std::string                 domain;
                    std::string                 source_identifier;
                    LoggingKeyValueStringMap    map_string_value;
                    LoggingKeyValueInt64Map     map_int64_value;
                    LoggingKeyValueInt32Map     map_int32_value;
                    LoggingKeyValueDoubleMap    map_double_value;
                    LoggingKeyValueBoolMap      map_bool_value;
                };
                
                class LoggingDataAccess:
                public chaos::service_common::persistence::data_access::AbstractDataAccess {

                public:
                    DECLARE_DA_NAME
                    
                    //! default constructor
                    LoggingDataAccess();
                    
                    //!default destructor
                    ~LoggingDataAccess();
                    
                    //! Insert a new entry in the log
                    /*!
                     \param structure that describe the log entry
                     */
                    virtual int insertNewEntry(LogEntry& log_entry) = 0;

                };
            }
        }
    }
}

#endif /* __CHAOSFramework__LoggingDataAccess_h */
