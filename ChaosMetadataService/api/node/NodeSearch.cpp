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
using namespace chaos::metadata_service::persistence::data_access;

NodeSearch::NodeSearch():
AbstractApi("nodeSearch"){
    
}

NodeSearch::~NodeSearch(){
    
}

chaos::common::data::CDataWrapper *NodeSearch::execute(chaos::common::data::CDataWrapper *api_data,
                                                       bool &detach_data) throw(chaos::CException) {
    CHECK_CDW_THROW_AND_LOG(api_data, NS_ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(api_data, "unique_id_filter", NS_ERR, -2, CHAOS_FORMAT("The %1% key is mandatory",%"unique_id_filter"));
    CHECK_KEY_THROW_AND_LOG(api_data, "node_type_filter", NS_ERR, -3, CHAOS_FORMAT("The %1% key is mandatory",%"node_type_filter"));
    
    bool alive_only = false;
    uint32_t last_sequence_id=0;
    uint32_t page_length=30;
    chaos::common::data::CDataWrapper *result=NULL;
    
    
    if (api_data->hasKey("last_node_sequence_id") &&
        api_data->isInt32Value("last_node_sequence_id") ){
        last_sequence_id=(uint32_t) api_data->getInt32Value("last_node_sequence_id");
        NS_DBG << "Need to load a new page starting from id:" << last_sequence_id;
    }
    
    if (api_data->hasKey("result_page_length") &&
        api_data->isInt32Value("result_page_length")){
        page_length=(uint32_t) api_data->getInt32Value("result_page_length");
        NS_DBG << "The length of the page has been set to:" << page_length;
    }
    if (api_data->hasKey("alive_only") &&
        api_data->isBoolValue("alive_only")){
        alive_only=api_data->getBoolValue("alive_only");
        NS_DBG << "Search only item that are with alive state:" << alive_only;
    }
    //get node data access
    GET_DATA_ACCESS(NodeDataAccess, n_da, -4)
    if (n_da->searchNode(&result,
                         api_data->getStringValue("unique_id_filter"),
                         api_data->getInt32Value("node_type_filter"),
                         alive_only,
                         last_sequence_id,
                         page_length)){
        LOG_AND_TROW(NS_ERR, -5, "Loading search page")
    }
    return result;
}
