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
#include "GetNodeNetworkAddress.h"

#define N_GNNA_INFO INFO_LOG(GetNodeNetworkAddress)
#define N_GNNA_DBG  DBG_LOG(GetNodeNetworkAddress)
#define N_GNNA_ERR  ERR_LOG(GetNodeNetworkAddress)

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::node;
using namespace chaos::metadata_service::persistence::data_access;

GetNodeNetworkAddress::GetNodeNetworkAddress():
AbstractApi("getNodeNetworkAddress"){
    
}

GetNodeNetworkAddress::~GetNodeNetworkAddress() {
    
}

chaos::common::data::CDataWrapper *GetNodeNetworkAddress::execute(chaos::common::data::CDataWrapper *api_data,
                                                                  bool& detach_data) throw(chaos::CException) {
    CHECK_CDW_THROW_AND_LOG(api_data, N_GNNA_ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(api_data, NodeDefinitionKey::NODE_UNIQUE_ID, N_GNNA_ERR, -2, "The ndk_unique_id key is mandatory")
    
    int err = 0;
    bool presence = 0;
    CDataWrapper *result = 0;
    const std::string node_unique_id = api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    GET_DATA_ACCESS(NodeDataAccess, n_da, -3)
    
    if((err = n_da->checkNodePresence(presence,
                                      node_unique_id))) {
        LOG_AND_TROW(N_GNNA_ERR, err, "Error checking node presence")
    }
    
    if(presence){
        if((err = n_da->getNodeDescription(node_unique_id, &result))) {
            LOG_AND_TROW(N_GNNA_ERR, err, "Error fetching node decription")
        }
    } else {
        LOG_AND_TROW(N_GNNA_ERR, -3, "Node not found")
    }
    return NULL;
}