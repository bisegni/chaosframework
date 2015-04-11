/*
 *	NodeSearch.cpp
 *	!CHAOS
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
#include "NodeSearch.h"
#define NS_INFO INFO_LOG(NodeSearch)
#define NS_DBG  DBG_LOG(NodeSearch)
#define NS_ERR  ERR_LOG(NodeSearch)
using namespace chaos::common::data;
using namespace chaos::metadata_service::api::node;

NodeSearch::NodeSearch():
AbstractApi("nodeSearch"){
    
}

NodeSearch::~NodeSearch() {
    
}

chaos::common::data::CDataWrapper *NodeSearch::execute(chaos::common::data::CDataWrapper *api_data,
                                                       bool& detach_data) throw(chaos::CException) {
    uint32_t last_sequence_id = 0;
    uint32_t page_length = 30;
    chaos::common::data::CDataWrapper *result;
    if(!api_data) throw CException(-1, "Search parameter are needed", __PRETTY_FUNCTION__);
    if(!api_data->hasKey("unique_id_filter")) throw CException(-2, "Filter for node unique id is madnatory", __PRETTY_FUNCTION__);
    if(!api_data->hasKey("node_type_filter")) throw CException(-3, "Filter for node type is madnatory", __PRETTY_FUNCTION__);
    if(api_data->hasKey("last_node_sequence_id")) {
        last_sequence_id = (uint32_t)api_data->getInt32Value("last_node_sequence_id");
        NS_DBG << "Need to load a new page starting from id:" << last_sequence_id;
    }
    if(api_data->hasKey("result_page_length")) {
        page_length = (uint32_t)api_data->getInt32Value("result_page_length");
        NS_DBG << "The length of the page has been set to:" << page_length;
    }
        //get node data access
    GET_DATA_ACCESS(persistence::data_access::NodeDataAccess, n_da, -4)
    if(n_da->searchNode(&result,
                        api_data->getStringValue("unique_id_filter"),
                        api_data->getInt32Value("node_type_filter"),
                        last_sequence_id,
                        page_length)) {
        LOG_AND_TROW(NS_ERR, -5, "Loading search page")
    }
    return result;
}