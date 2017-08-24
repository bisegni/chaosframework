/*
 * Copyright 2012, 2017 INFN
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