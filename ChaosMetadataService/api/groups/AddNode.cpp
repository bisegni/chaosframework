/*
 *	AddNode.h
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

#include "AddNode.h"

using namespace chaos::metadata_service::api::groups;

#define G_AN_INFO INFO_LOG(AddNode)
#define G_AN_DBG  DBG_LOG(AddNode)
#define G_AN_ERR  ERR_LOG(AddNode)

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::groups;
using namespace chaos::metadata_service::persistence::data_access;

AddNode::AddNode():
AbstractApi("addNode"){
    
}

AddNode::~AddNode() {
    
}

chaos::common::data::CDataWrapper *AddNode::execute(chaos::common::data::CDataWrapper *api_data, bool& detach_data) {
    int err = 0;
    CHECK_CDW_THROW_AND_LOG(api_data, G_AN_ERR, -1, "No parameter found");
    CHECK_KEY_THROW_AND_LOG(api_data, "node_name", G_AN_ERR, -2, "The node_name key is mandatory");
    CHAOS_LASSERT_EXCEPTION(api_data->isStringValue("node_name"), G_AN_ERR, -3, "The node_name needs to be a string");
        
    CHECK_KEY_THROW_AND_LOG(api_data, "node_domain", G_AN_ERR, -4, "The node_domain key is mandatory");
    CHAOS_LASSERT_EXCEPTION((api_data->isStringValue("node_domain") && api_data->getStringValue("node_domain").size()), G_AN_ERR, -5, "The node_domain needs to be a non empty string");

    GET_DATA_ACCESS(TreeGroupDataAccess, tg_da, -6);
    
    const std::string node_name = api_data->getStringValue("node_name");
    const std::string node_domain = api_data->getStringValue("node_domain");
    if(api_data->hasKey("node_parent_path")) {
        
        const std::string node_path = api_data->getStringValue("node_parent_path");
        
        if((err = tg_da->addNewNodeGroupToDomain(node_domain,
                                                 node_name,
                                                 node_path))) {
            LOG_AND_TROW_FORMATTED(G_AN_ERR, -6, "Error creating node %1% in path %2% with error code %3%(%4)", %node_name%node_path%err%chaos::error::getErrorMessage(err));
        }
    } else {
        if((err = tg_da->addNewNodeGroupToDomain(node_domain,
                                                 node_name))) {
            LOG_AND_TROW_FORMATTED(G_AN_ERR, -6, "Error creating root node %1% with error code %2%(%3%)", %node_name%err%chaos::error::getErrorMessage(err));
        }
    }
    return NULL;
}