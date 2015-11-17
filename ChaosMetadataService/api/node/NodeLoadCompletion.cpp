/*
 *	NodeLoadCompletion.cpp
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
    
    GET_DATA_ACCESS(ControlUnitDataAccess, cu_da, -4)
    
    cu_id = api_data->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID);
    
    CU_LC_INFO << "Load completed for control unit "<<cu_id;

    if((err = cu_da->getInstanceDescription(cu_id, &tmp_ptr))) {
        LOG_AND_TROW_FORMATTED(CU_LC_ERR, err, "Error %1% durring fetch of instance for unit server %2%", %err%cu_id)
    } else if(tmp_ptr) {
        std::auto_ptr<CDataWrapper> auto_inst(tmp_ptr);
        bool auto_init = auto_inst->hasKey("auto_init")?auto_inst->getBoolValue("auto_init"):false;
        bool auto_start = auto_inst->hasKey("auto_start")?auto_inst->getBoolValue("auto_start"):false;
        
        if(auto_init || auto_start) {
            uint32_t sandbox_index = getBatchExecutor()->getNextSandboxToUse();
            if(auto_init){
                std::auto_ptr<CDataWrapper> init_datapack(new CDataWrapper());
                init_datapack->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, cu_id);
                init_datapack->addInt32Value("action", (int32_t)0);
                
                command_id = getBatchExecutor()->submitCommand(GET_MDS_COMMAND_ALIAS(batch::control_unit::IDSTControlUnitBatchCommand),
                                                               init_datapack.release(),
                                                               sandbox_index,
                                                               10);
                
                CU_LC_INFO << "Submitted init command for control unit "<<cu_id<< " with id:" << command_id << " on sandbox:" << sandbox_index;
            }
            if(auto_start){
                std::auto_ptr<CDataWrapper> start_datapack(new CDataWrapper());
                start_datapack->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, cu_id);
                start_datapack->addInt32Value("action", (int32_t)1);
                command_id = getBatchExecutor()->submitCommand(GET_MDS_COMMAND_ALIAS(batch::control_unit::IDSTControlUnitBatchCommand),
                                                               start_datapack.release(),
                                                               sandbox_index,
                                                               0);
                CU_LC_INFO << "Submitted start command for control unit "<<cu_id<< " with id:" << command_id << " on sandbox:" << sandbox_index;
            }
            
        }
    }
    return NULL;
}