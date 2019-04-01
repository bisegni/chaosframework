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

#include <chaos/common/io/QueryIndexCursor.h>
#include <chaos/common/io/IODirectIODriver.h>
#include <chaos/common/global.h>
#define INFO    INFO_LOG(QueryIndexCursor)
#define DBG     DBG_LOG(QueryIndexCursor)
#define ERR     ERR_LOG(QueryIndexCursor)

using namespace chaos::common::io;
using namespace chaos::common::data;
using namespace chaos::common::network;
using namespace chaos::common::direct_io::channel;
using namespace chaos::common::direct_io::channel::opcode_headers;

QueryIndexCursor::QueryIndexCursor(const std::string& _query_id,
                                   chaos::common::network::URLServiceFeeder& _connection_feeder,
                                   const std::string& _node_id,
                                   uint64_t _start_ts,
                                   uint64_t _end_ts,
                                   uint32_t page_len):
QueryCursor(_query_id,
            _connection_feeder,
            _node_id,
            _start_ts,
            _end_ts,
            page_len){}

QueryIndexCursor::QueryIndexCursor(const std::string&                        _query_id,
                                   chaos::common::network::URLServiceFeeder& _connection_feeder,
                                   const std::string&                        _node_id,
                                   uint64_t                                  _start_ts,
                                   uint64_t                                  _end_ts,
                                   const ChaosStringSet&                     _meta_tags,
                                   uint32_t                                  page_len):
QueryCursor(_query_id,
            _connection_feeder,
            _node_id,
            _start_ts,
            _end_ts,
            _meta_tags,
            page_len){}

QueryIndexCursor::QueryIndexCursor(const std::string&                        _query_id,
                                   chaos::common::network::URLServiceFeeder& _connection_feeder,
                                   const std::string&                        _node_id,
                                   uint64_t                                  _start_ts,
                                   uint64_t                                  _end_ts,
                                   uint64_t                                  _sequid,
                                   uint64_t                                  _runid,
                                   uint32_t                                  page_len):
QueryCursor(_query_id,
            _connection_feeder,
            _node_id,
            _start_ts,
            _end_ts,
            _sequid,
            _runid,
            page_len){}

QueryIndexCursor::QueryIndexCursor(const std::string& _query_id,
                                   chaos::common::network::URLServiceFeeder& _connection_feeder,
                                   const std::string& _node_id,
                                   uint64_t _start_ts,
                                   uint64_t _end_ts,
                                   uint64_t _sequid,
                                   uint64_t _runid,
                                   const ChaosStringSet& _meta_tags,
                                   uint32_t page_len):
QueryCursor(_query_id,
            _connection_feeder,
            _node_id,
            _start_ts,
            _end_ts,
            _sequid,
            _runid,
            _meta_tags,
            page_len){}

int QueryIndexCursor::fetchData() {
    IODirectIODriverClientChannels *next_client = NULL;
    if((next_client = static_cast<IODirectIODriverClientChannels*>(connection_feeder.getService())) == NULL) return -1;
    if((api_error = next_client->device_client_channel->queryDataCloud(node_id,
                                                                       meta_tags,
                                                                       start_ts,
                                                                       end_ts,
                                                                       page_len,
                                                                       result_page.last_record_found_seq,
                                                                       result_page.found_element_page,
                                                                       true))) {
        ERR << CHAOS_FORMAT("Error during fetching query page with code %1%", %api_error);
        phase = QueryPhaseEnded;
        return api_error;
    } else {
        result_page.current_fetched = 0;
        //now we need to fetch every element
        CDWShrdPtr found_data;
        VectorCDWShrdPtr index_found = result_page.found_element_page;
        result_page.found_element_page.clear();
        for(QueryResultPageIterator it = index_found.begin(),
            end = index_found.end();
            it < end;
            it++){
            if((api_error = next_client->device_client_channel->getDataByIndex(*it,
                                                                               found_data)) == 0) {
                result_page.found_element_page.push_back(found_data);
            }
        }
        if(result_page.found_element_page.size() < page_len) {
            phase = QueryPhaseEnded;
        }
    }
    return api_error;
}
