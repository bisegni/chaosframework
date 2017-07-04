/*
 *	QueryCursor.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 06/09/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__CE8768D_5BB0_4EF2_A186_7685A0C31B3F_QueryCursor_h
#define __CHAOSFramework__CE8768D_5BB0_4EF2_A186_7685A0C31B3F_QueryCursor_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/exception/CException.h>
#include <chaos/common/network/URLServiceFeeder.h>
#include <chaos/common/direct_io/channel/DirectIODeviceClientChannel.h>

#include <stdint.h>

namespace chaos {
    namespace common {
        namespace io {
            class IODirectIODriver;
            
            typedef enum QueryPhase {
                QueryPhaseNotStarted,
                QueryPhaseStarted,
                QueryPhaseEnded
            } QueryPhase;
            
            CHAOS_DEFINE_VECTOR_FOR_TYPE(ChaosSharedPtr<chaos::common::data::CDataWrapper>, ResultPageDecodedPacket);
            
#define DEFAULT_PAGE_LEN 100
            class QueryCursor {
                friend class IODirectIODriver;
                
                struct ResultPage {
                    unsigned int current_fetched;
                    direct_io::channel::opcode_headers::SearchSequence last_record_found_seq;
                    direct_io::channel::opcode_headers::QueryResultPage found_element_page;

                    ResultPage();
                    ~ResultPage();
                    const bool hasNext() const;
                    
                    ChaosSharedPtr<chaos::common::data::CDataWrapper> next()  throw (chaos::CException);
                };
                
                const std::string query_id;
                const std::string node_id;
                uint64_t start_ts;
                uint64_t end_ts;
                //!is the reuslt page dimension
                uint32_t page_len;
                QueryPhase phase;
                ResultPage result_page;
                int64_t api_error;
                chaos::common::network::URLServiceFeeder& connection_feeder;
                
                QueryCursor(const std::string& _query_id,
                            chaos::common::network::URLServiceFeeder& _connection_feeder,
                            const std::string& _node_id,
                            uint64_t _start_ts,
                            uint64_t _end_ts,
                            uint32_t page_len=DEFAULT_PAGE_LEN);
                ~QueryCursor();
                
                int64_t fetchNewPage();
                
            public:
                const std::string& queryID() const;
                
                const bool hasNext();
                const int32_t getError();
                ChaosSharedPtr<chaos::common::data::CDataWrapper> next() throw (CException);
                
                const uint32_t getPageLen() const;
                
                void setPageDimension(uint32_t new_page_len);
            };
            
        }
    }
}

#endif /* __CHAOSFramework__CE8768D_5BB0_4EF2_A186_7685A0C31B3F_QueryCursor_h */
