/*
 *	NodeCreateUnitServer.cpp
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
#include "NodeCreateUnitServer.h"

#define USRA_INFO INFO_LOG(NodeRegister)
#define USRA_DBG  DBG_LOG(NodeRegister)
#define USRA_ERR  ERR_LOG(NodeRegister)

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::node;

NodeCreateUnitServer::NodeCreateUnitServer():
AbstractApi("newNode"){
    
}

NodeCreateUnitServer::~NodeCreateUnitServer() {
    
}

chaos::common::data::CDataWrapper *NodeCreateUnitServer::execute(chaos::common::data::CDataWrapper *api_data,
                                                                 bool& detach_data) throw(chaos::CException) {
    bool presence = false;
    chaos::common::data::CDataWrapper *result = NULL;
    if(!api_data->hasKey(NodeDefinitionKey::NODE_UNIQUE_ID)) {
        LOG_AND_TROW(USRA_ERR, -1, "Node unique id not found")
    }
    
    if(!api_data->hasKey(NodeDefinitionKey::NODE_TYPE)) {
        throw CException(-2, "Node type not found", __PRETTY_FUNCTION__);
    }
    
    const std::string node_type = api_data->getStringValue(NodeDefinitionKey::NODE_TYPE);
    if(node_type.compare(NodeType::NODE_TYPE_UNIT_SERVER) != 0) {
        throw CException(-3, "Note type is not an Unit Server", __PRETTY_FUNCTION__);
    }
    
    //!get the unit server data access
    persistence::data_access::UnitServerDataAccess *us_da = getPersistenceDriver()->getDataAccess<persistence::data_access::UnitServerDataAccess>();
    if(us_da->checkUSPresence(api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID), presence)) {
        
    }
    return result;
}