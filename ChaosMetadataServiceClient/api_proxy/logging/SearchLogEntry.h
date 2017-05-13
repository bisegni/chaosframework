/*
 *	SearchLogEntry.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 25/02/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__SearchLogEntry_h
#define __CHAOSFramework__SearchLogEntry_h


#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>
#include <ChaosMetadataServiceClient/api_proxy/logging/logging_types.h>
#include <ChaosMetadataServiceClient/api_proxy/logging/GetLogForSourceUID.h>

#include <chaos/common/chaos_types.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace logging {
                
                class SearchLogEntryHelper:
                public GetLogForSourceUIDHelper{
                public:
                    SearchLogEntryHelper(chaos::common::data::CDataWrapper *api_result);
                    ~SearchLogEntryHelper();
                    size_t getLogEntryListSize();
                    const LogEntryList& getLogEntryList();
                };
                
                //! get log entries for a source
                class SearchLogEntry:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(SearchLogEntry)
                protected:
                    API_PROXY_CD_DECLARATION(SearchLogEntry)
                public:

                    //! Perform an advanced search specifind also the range of ts
                    /*!
                     \param search_string is the string that is search with a 'like' procedure in all fields
                     \param domain is the lis tof domain to wich need to belog the result entry
                     \param start_ts if > 0 is used has start timestamp
                     \param end_ts if > 0 is used as end timestamp
                     \param last_sequence_id is the id of the last returned entries in the past query
                     \param page_length is the length of the returned element
                     */
                    ApiProxyResult execute(const std::string& search_string,
                                           const LogDomainList& domain_list,
                                           const uint64_t start_ts,
                                           const uint64_t end_ts,
                                           const uint64_t last_sequence_id,
                                           const uint32_t page_length = 30);
                    static std::unique_ptr<SearchLogEntryHelper> getHelper(chaos::common::data::CDataWrapper *api_result);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__SearchLogEntry_h */
