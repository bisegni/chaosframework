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

using namespace chaos::common::io;
using namespace chaos::common::data;
using namespace chaos::common::network;
using namespace chaos::common::direct_io::channel;
using namespace chaos::common::direct_io::channel::opcode_headers;

int64_t QueryIndexCursor::fetchData() {
    IODirectIODriverClientChannels *next_client = NULL;
    if((next_client = static_cast<IODirectIODriverClientChannels*>(connection_feeder.getService())) == NULL) return -1;
//    if((api_error = next_client->device_client_channel->queryDataCloud(node_id,
//                                                                       meta_tags,
//                                                                       start_ts,
//                                                                       end_ts,
//                                                                       page_len,
//                                                                       result_page.last_record_found_seq,
//                                                                       result_page.found_element_page))) {
//        ERR << CHAOS_FORMAT("Error during fetching query page with code %1%", %api_error);
//        phase = QueryPhaseEnded;
//        return api_error;
//    } else {
//        result_page.current_fetched = 0;
//        if(result_page.found_element_page.size() < page_len) {
//            phase = QueryPhaseEnded;
//        }
//    }
    return api_error;
}
