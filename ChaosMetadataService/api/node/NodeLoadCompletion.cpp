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

#include "NodeLoadCompletion.h"

#include <chaos/common/chaos_constants.h>

#include <boost/algorithm/string/predicate.hpp>

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::node;
using namespace chaos::metadata_service::persistence::data_access;

#define CU_LC_INFO INFO_LOG(NodeLoadCompletion)
#define CU_LC_DBG  DBG_LOG(NodeLoadCompletion)
#define CU_LC_ERR  ERR_LOG(NodeLoadCompletion)

NodeLoadCompletion::NodeLoadCompletion():
AbstractApi(chaos::MetadataServerNodeDefinitionKeyRPC::ACTION_NODE_LOAD_COMPLETION){
    
}

NodeLoadCompletion::~NodeLoadCompletion() {
    
}

CDataWrapper *NodeLoadCompletion::execute(CDataWrapper *api_data,
                                          bool& detach_data) throw(chaos::CException) {
    chaos::common::data::CDataWrapper *result = NULL;
    
    CHECK_CDW_THROW_AND_LOG(api_data, CU_LC_ERR, -1, "No parameter found");
    CHECK_KEY_THROW_AND_LOG(api_data, chaos::NodeDefinitionKey::NODE_UNIQUE_ID , CU_LC_ERR, -2, "The ndk_uid key is mandatory");
    if(!api_data->isStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID)){LOG_AND_TROW(CU_LC_ERR, -3, "The ndk_uid key need to be string");}
    
    CHECK_KEY_THROW_AND_LOG(api_data, chaos::NodeDefinitionKey::NODE_TYPE , CU_LC_ERR, -3, "The ndk_type key is mandatory");
    if(!api_data->isStringValue(chaos::NodeDefinitionKey::NODE_TYPE)){LOG_AND_TROW(CU_LC_ERR, -3, "The ndk_type key need to be string");}

    CHECK_KEY_THROW_AND_LOG(api_data, chaos::NodeDefinitionKey::NODE_RPC_ADDR , CU_LC_ERR, -3, "The ndk_rpc_addr key is mandatory");
    if(!api_data->isStringValue(chaos::NodeDefinitionKey::NODE_RPC_ADDR)){LOG_AND_TROW(CU_LC_ERR, -3, "The ndk_rpc_addr key need to be string");}
    
    const std::string node_type = api_data->getStringValue(NodeDefinitionKey::NODE_TYPE);
    if(node_type.compare(NodeType::NODE_TYPE_UNIT_SERVER) == 0) {
        
    } else if(boost::starts_with(node_type, NodeType::NODE_TYPE_CONTROL_UNIT)) {
        //the control units type value start with the default but are followe by custom type
        result = controlUnitCompletion(api_data,
                                         detach_data);
    } else {
        throw CException(-3, "Type of node not managed for registration", __PRETTY_FUNCTION__);
    }

    return result;
}

//! perform the completion for control unit type node
chaos::common::data::CDataWrapper *NodeLoadCompletion::controlUnitCompletion(chaos::common::data::CDataWrapper *api_data,
                                                                               bool& detach_data) throw(chaos::CException) {
    int                                         err                 = 0;
    uint64_t                                    command_id          = 0;
    CDataWrapper                                *tmp_ptr            = NULL;
    std::string                                 temp_node_uid       = "";
    std::string                                 cu_id               = "";
    
    return NULL;
}