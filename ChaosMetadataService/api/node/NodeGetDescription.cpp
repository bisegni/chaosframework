/*
 *	NodeGetDescription.cpp
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
#include "NodeGetDescription.h"

#define USRA_INFO INFO_LOG(NodeRegister)
#define USRA_DBG  DBG_LOG(NodeRegister)
#define USRA_ERR  ERR_LOG(NodeRegister)

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::node;

NodeGetDescription::NodeGetDescription():
AbstractApi("getNodeDescription"){
    
}

NodeGetDescription::~NodeGetDescription() {
    
}

chaos::common::data::CDataWrapper *NodeGetDescription::execute(chaos::common::data::CDataWrapper *api_data,
                                                                 bool& detach_data) throw(chaos::CException) {
    int err = 0;
    bool presence = false;
    chaos::common::data::CDataWrapper *result = NULL;
    if(!api_data->hasKey(NodeDefinitionKey::NODE_UNIQUE_ID)) {
        LOG_AND_TROW(USRA_ERR, -1, "Node unique id not set as parameter")
    }
    
    //!get the unit server data access
    persistence::data_access::NodeDataAccess *n_da = getPersistenceDriver()->getDataAccess<persistence::data_access::NodeDataAccess>();
    if((err = n_da->checkNodePresence(presence,
                                      api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID)))) {
        LOG_AND_TROW(USRA_ERR, err, "Error checking node presence")
    } else if(presence){
        if((err = n_da->getNodeDescription(api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID), &result))) {
            LOG_AND_TROW(USRA_ERR, err, "Error fetching node decription")
        }
    } else {
        LOG_AND_TROW(USRA_ERR, -2, "Node not found")
    }
    return result;
}