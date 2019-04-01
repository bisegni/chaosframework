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
            protected:
                struct ResultPage {
                    unsigned int current_fetched;
                    direct_io::channel::opcode_headers::SearchSequence last_record_found_seq;
                    direct_io::channel::opcode_headers::QueryResultPage found_element_page;

                    ResultPage();
                    ~ResultPage();
                    const bool hasNext() const;
                    CUInt32 size() const;
                    ChaosSharedPtr<chaos::common::data::CDataWrapper> next()  ;
                };
                
                const std::string query_id;
                const std::string node_id;
                CUInt64 start_ts;
                CUInt64 end_ts;
                CUInt64 start_seq;
                CUInt64 runid_seq;
                const ChaosStringSet meta_tags;
                //!is the reuslt page dimension
                CUInt32 page_len;
                QueryPhase phase;
                ResultPage result_page;
                int api_error;
                chaos::common::network::URLServiceFeeder& connection_feeder;
                
                QueryCursor(const std::string& _query_id,
                            chaos::common::network::URLServiceFeeder& _connection_feeder,
                            const std::string& _node_id,
                            CUInt64 _start_ts,
                            CUInt64 _end_ts,
                            CUInt32 page_len=DEFAULT_PAGE_LEN);

                QueryCursor(const std::string&                        _query_id,
                            chaos::common::network::URLServiceFeeder& _connection_feeder,
                            const std::string&                        _node_id,
                            CUInt64                                  _start_ts,
                            CUInt64                                  _end_ts,
                            const ChaosStringSet&                     _meta_tags,
                            CUInt32                                  page_len = DEFAULT_PAGE_LEN);

                QueryCursor(const std::string&                        _query_id,
                            chaos::common::network::URLServiceFeeder& _connection_feeder,
                            const std::string&                        _node_id,
                            CUInt64                                  _start_ts,
                            CUInt64                                  _end_ts,
                            CUInt64                                  _sequid,
                            CUInt64                                  _runid,
                            CUInt32                                  page_len = DEFAULT_PAGE_LEN);

                QueryCursor(const std::string& _query_id,
                            chaos::common::network::URLServiceFeeder& _connection_feeder,
                            const std::string& _node_id,
                            CUInt64 _start_ts,
                            CUInt64 _end_ts,
                            CUInt64 _sequid,
                            CUInt64 _runid,
                            const ChaosStringSet& _meta_tags,
                            CUInt32 page_len=DEFAULT_PAGE_LEN);
                ~QueryCursor();
                
                int fetchNewPage();
                virtual int fetchData();
            public:
                const std::string& queryID() const;
                
                const bool hasNext();
                const int32_t getError();
                ChaosSharedPtr<chaos::common::data::CDataWrapper> next();
                CUInt32 size()const;
                const CUInt32 getPageLen() const;
                
                void setPageDimension(CUInt32 new_page_len);
                void getIndexes(CUInt64& runid,CUInt64& seqid);
            };
            
        }
    }
}

#endif /* __CHAOSFramework__CE8768D_5BB0_4EF2_A186_7685A0C31B3F_QueryCursor_h */
