/*
 *	GetLogForSourceUID.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 15/02/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__GetLogForSourceUID_h
#define __CHAOSFramework__GetLogForSourceUID_h

#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

#include <chaos/common//chaos_types.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace logging {

                
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
                
                CHAOS_DEFINE_VECTOR_FOR_TYPE(boost::shared_ptr<LogEntry>, LogEntryList);
                
                class GetLogForSourceUIDHelper {
                    LogEntryList log_entry_list;
                public:
                    GetLogForSourceUIDHelper(chaos::common::data::CDataWrapper *api_result);
                    ~GetLogForSourceUIDHelper();
                    size_t getLogEntryListSize();
                    const LogEntryList& getLogEntryList();
                };
                
                //! get log entries for a source
                class GetLogForSourceUID:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(GetLogForSourceUID)
                protected:
                    API_PROXY_CD_DECLARATION(GetLogForSourceUID)
                public:
                    //! Add a new node in the domain
                    /*!
                     If the node_parent_path is an empty string(the default value) the ndoe became a root in the domain
                     \param source node that has emitted the log entries
                     \param domain if is not and empty string it will be used to filter domain in log entries
                     \param last_sequence_id is the id of the last returned entries in the past query
                     */
                    ApiProxyResult execute(const std::string& source,
                                           const uint64_t last_sequence_id = 0,
                                           const std::string& domain = std::string(),
                                           const uint32_t page_length = 30);
                    
                    static std::auto_ptr<GetLogForSourceUIDHelper> getHelper(chaos::common::data::CDataWrapper *api_result);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__GetLogForSourceUID_h */
