/*
 *	ChangeNodeState.cpp
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
#include "ChangeNodeState.h"

#define N_CS_INFO INFO_LOG(ChangeNodeState)
#define N_CS_DBG  DBG_LOG(ChangeNodeState)
#define N_CS_ERR  ERR_LOG(ChangeNodeState)

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::node;
using namespace chaos::metadata_service::persistence::data_access;

ChangeNodeState::ChangeNodeState():
AbstractApi("changeNodeState"){
    
}

ChangeNodeState::~ChangeNodeState() {
    
}

chaos::common::data::CDataWrapper *ChangeNodeState::execute(chaos::common::data::CDataWrapper *api_data,
                                                               bool& detach_data) throw(chaos::CException) {
    CHECK_CDW_THROW_AND_LOG(api_data, N_CS_ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(api_data, NodeDefinitionKey::NODE_UNIQUE_ID, N_CS_ERR, -2, "The ndk_unique_id key is mandatory")
    CHECK_KEY_THROW_AND_LOG(api_data, "to_state", N_CS_ERR, -3, "the to_state parameter is mandatory")

    std::string node_unique_id = api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    GET_DATA_ACCESS(NodeDataAccess, n_da, -3)
    return NULL;
}