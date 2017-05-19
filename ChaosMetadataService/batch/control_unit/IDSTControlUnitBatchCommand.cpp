/*
 *	IDSTControlUnitBatchCommand.cpp
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

#include "IDSTControlUnitBatchCommand.h"

#include "../../common/CUCommonUtility.h"

using namespace chaos::common::data;
using namespace chaos::common::network;
using namespace chaos::metadata_service::batch::control_unit;

#define CU_IDST_BC_INFO INFO_LOG(IDSTControlUnitBatchCommand)
#define CU_IDST_BC_DBG  DBG_LOG(IDSTControlUnitBatchCommand)
#define CU_IDST_BC_ERR  ERR_LOG(IDSTControlUnitBatchCommand)

#define MOVE_STRING_VALUE(k, src, dst)\
if(src->hasKey(k)) {\
dst->addStringValue(k, src->getStringValue(k));\
}

#define MERGE_STRING_VALUE(k, src, src2, dst)\
if(src2->hasKey(k)) {\
dst->addStringValue(k, src2->getStringValue(k));\
} else {\
MOVE_STRING_VALUE(k, src, dst)\
}

#define MOVE_INT32_VALUE(k, src, dst)\
if(src->hasKey(k)) {\
dst->addInt32Value(k, src->getInt32Value(k));\
}


DEFINE_MDS_COMAMND_ALIAS(IDSTControlUnitBatchCommand)


IDSTControlUnitBatchCommand::IDSTControlUnitBatchCommand():
MDSBatchCommand(),
retry_number(0),
cu_id(),
action(ACTION_INIT) {}

IDSTControlUnitBatchCommand::~IDSTControlUnitBatchCommand() {}

// inherited method
void IDSTControlUnitBatchCommand::setHandler(CDataWrapper *data) {
    MDSBatchCommand::setHandler(data);
    
    int err = 0;
    CDataWrapper *tmp_ptr = NULL;
    
    //set cu id to the batch command datapack
    cu_id = data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    action = static_cast<IDSTAction>(data->getInt32Value("action"));//0-init,1-start,2-ztop,3-deinit
    
    if((err = getDataAccess<mds_data_access::NodeDataAccess>()->getNodeDescription(cu_id, &tmp_ptr))) {
        LOG_AND_TROW(CU_IDST_BC_ERR, -1, "Control Unit information has not been found!")
    } else if(tmp_ptr != NULL) {
        ChaosUniquePtr<chaos::common::data::CDataWrapper> auto_node_info(tmp_ptr);
        if(auto_node_info->hasKey(NodeDefinitionKey::NODE_RPC_ADDR) &&
           auto_node_info->hasKey(NodeDefinitionKey::NODE_RPC_DOMAIN) ){
            request = createRequest(auto_node_info->getStringValue(NodeDefinitionKey::NODE_RPC_ADDR),
                                    auto_node_info->getStringValue(NodeDefinitionKey::NODE_RPC_DOMAIN),
                                    "");
        }else{
            LOG_AND_TROW(CU_IDST_BC_ERR, -2, "Control Unit information doesn't has the RPC address!")
        }
        
    } else {
        LOG_AND_TROW(CU_IDST_BC_ERR, -3, "Control Unit information not found!")
    }
}

// inherited method
void IDSTControlUnitBatchCommand::acquireHandler() {
    MDSBatchCommand::acquireHandler();
    switch(request->phase) {
        case MESSAGE_PHASE_UNSENT: {
            switch(action) {
                case ACTION_INIT:
                    CU_IDST_BC_INFO << "Send init command to " << cu_id;
                    request->remote_action = NodeDomainAndActionRPC::ACTION_NODE_INIT;
                    message = common::CUCommonUtility::initDataPack(cu_id,
                                                                    getDataAccess<mds_data_access::NodeDataAccess>(),
                                                                    getDataAccess<mds_data_access::ControlUnitDataAccess>(),
                                                                    getDataAccess<mds_data_access::DataServiceDataAccess>());
                    break;
                case ACTION_START:
                    CU_IDST_BC_INFO << "Send start command to " << cu_id;
                    request->remote_action = NodeDomainAndActionRPC::ACTION_NODE_START;
                    message = common::CUCommonUtility::startDataPack(cu_id);
                    break;
                case ACTION_STOP:
                    CU_IDST_BC_INFO << "Send stop command to " << cu_id;
                    request->remote_action = NodeDomainAndActionRPC::ACTION_NODE_STOP;
                    message = common::CUCommonUtility::stopDataPack(cu_id);
                    break;
                case ACTION_DEINIT:
                    CU_IDST_BC_INFO << "Send deinit command to " << cu_id;
                    request->remote_action = NodeDomainAndActionRPC::ACTION_NODE_DEINIT;
                    message = common::CUCommonUtility::deinitDataPack(cu_id);
                    break;
            }
            
            //send message for action
            sendMessage(*request,
                        message.get());
            BC_END_RUNNING_PROPERTY
            break;
        }
        case MESSAGE_PHASE_SENT:
        case MESSAGE_PHASE_COMPLETED:
        case MESSAGE_PHASE_TIMEOUT: {
            break;
        }
    }
}

// inherited method
void IDSTControlUnitBatchCommand::ccHandler() {
    MDSBatchCommand::ccHandler();
    switch(request->phase) {
        case MESSAGE_PHASE_UNSENT: {
            break;
        }
        case MESSAGE_PHASE_SENT: {
            manageRequestPhase(*request);
            break;
        }
            
        case MESSAGE_PHASE_COMPLETED:
        case MESSAGE_PHASE_TIMEOUT: {
            BC_END_RUNNING_PROPERTY
            break;
        }
    }
}
// inherited method
bool IDSTControlUnitBatchCommand::timeoutHandler() {
    bool result = MDSBatchCommand::timeoutHandler();
    return result;
}
