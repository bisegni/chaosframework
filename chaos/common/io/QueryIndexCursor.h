/*
 * Copyright 2012, 25/05/2018 INFN
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

#ifndef __CHAOSFramework__CE8768D_5BB0_4EF2_A186_7685A0C31B3F_QueryIndexCursor_h
#define __CHAOSFramework__CE8768D_5BB0_4EF2_A186_7685A0C31B3F_QueryIndexCursor_h

#include <chaos/common/io/QueryCursor.h>

namespace chaos {
    namespace common {
        namespace io {
            class QueryIndexCursor:
            public QueryCursor {
            protected:
                int fetchData();
            public:
                QueryIndexCursor(const std::string& _query_id,
                                 chaos::common::network::URLServiceFeeder& _connection_feeder,
                                 const std::string& _node_id,
                                 CUInt64 _start_ts,
                                 CUInt64 _end_ts,
                                 CUInt32 page_len=DEFAULT_PAGE_LEN);
                
                QueryIndexCursor(const std::string&                        _query_id,
                                 chaos::common::network::URLServiceFeeder& _connection_feeder,
                                 const std::string&                        _node_id,
                                 CUInt64                                  _start_ts,
                                 CUInt64                                  _end_ts,
                                 const ChaosStringSet&                     _meta_tags,
                                 CUInt32                                  page_len = DEFAULT_PAGE_LEN);
                
                QueryIndexCursor(const std::string&                        _query_id,
                                 chaos::common::network::URLServiceFeeder& _connection_feeder,
                                 const std::string&                        _node_id,
                                 CUInt64                                  _start_ts,
                                 CUInt64                                  _end_ts,
                                 CUInt64                                  _sequid,
                                 CUInt64                                  _runid,
                                 CUInt32                                  page_len = DEFAULT_PAGE_LEN);
                
                QueryIndexCursor(const std::string& _query_id,
                                 chaos::common::network::URLServiceFeeder& _connection_feeder,
                                 const std::string& _node_id,
                                 CUInt64 _start_ts,
                                 CUInt64 _end_ts,
                                 CUInt64 _sequid,
                                 CUInt64 _runid,
                                 const ChaosStringSet& _meta_tags,
                                 CUInt32 page_len=DEFAULT_PAGE_LEN);
            };
        }
    }
}


#endif /* QueryIndexCursor_hpp */
