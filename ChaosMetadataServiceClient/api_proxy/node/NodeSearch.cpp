/*
 *	NodeSearch.cpp
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
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

#include <ChaosMetadataServiceClient/api_proxy/node/NodeSearch.h>
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::node;

    //! default constructor
NodeSearch::NodeSearch(chaos::common::message::MultiAddressMessageChannel *_mn_message):
ApiProxy("nodeSearch", _mn_message) {

}
    //! default destructor
NodeSearch::~NodeSearch() {

}

/*!

 */
ApiProxyResult NodeSearch::execute(const std::string& unique_id_filter,
                                   unsigned int node_type_filter,
                                   unsigned int last_node_sequence_id,
                                   unsigned int page_length) {
    chaos::common::data::CDataWrapper message;
    message.addStringValue("unique_id_filter", unique_id_filter);
    message.addInt32Value("node_type_filter", node_type_filter);
    message.addInt32Value("last_node_sequence_id", last_node_sequence_id);
    message.addInt32Value("result_page_length", page_length);
    return callApi("system", getName(), &message);
}