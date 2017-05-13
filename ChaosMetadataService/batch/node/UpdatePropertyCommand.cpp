/*
 *	UpdatePropertyCommand.cpp
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

#include "UpdatePropertyCommand.h"

using namespace chaos::common::data;
using namespace chaos::common::network;
using namespace chaos::metadata_service::batch::node;

#define CU_UP_INFO INFO_LOG(UpdatePropertyCommand)
#define CU_UP_DBG  DBG_LOG(UpdatePropertyCommand)
#define CU_UP_ERR  ERR_LOG(UpdatePropertyCommand)

DEFINE_MDS_COMAMND_ALIAS(UpdatePropertyCommand)

UpdatePropertyCommand::UpdatePropertyCommand():
MDSBatchCommand() {}

UpdatePropertyCommand::~UpdatePropertyCommand() {}

// inherited method
void UpdatePropertyCommand::setHandler(CDataWrapper *data) {
    MDSBatchCommand::setHandler(data);
    int err = 0;
    CDataWrapper *tmp_ptr = NULL;
    
    CHECK_CDW_THROW_AND_LOG(data, CU_UP_ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(data, NodeDefinitionKey::NODE_UNIQUE_ID, CU_UP_ERR, -2, "The attribute node unique id is mandatory")
    CHECK_KEY_THROW_AND_LOG(data, "update_property", CU_UP_ERR, -3, "The attribute node unique id is mandatory")

    update_property_pack.reset(data->getCSDataValue("update_property"));
    
    const std::string node_uid = data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    //fetch data for sending message to node id
    
    if((err = getDataAccess<mds_data_access::NodeDataAccess>()->getNodeDescription(node_uid, &tmp_ptr))) {
        LOG_AND_TROW_FORMATTED(CU_UP_ERR, err, "Error loading infomation for node '%1%'", %node_uid)
    } else if(!tmp_ptr) {
        LOG_AND_TROW_FORMATTED(CU_UP_ERR, -1, "No description found for node '%1%'", %node_uid)
    }
    
    std::unique_ptr<CDataWrapper> node_description(tmp_ptr);
    const std::string node_rpc_address = node_description->getStringValue(NodeDefinitionKey::NODE_RPC_ADDR);
    const std::string node_rpc_domain = node_description->getStringValue(NodeDefinitionKey::NODE_RPC_DOMAIN);
    request = createRequest(node_rpc_address,
                            node_rpc_domain,
                            NodeDomainAndActionRPC::ACTION_UPDATE_PROPERTY);
}

// inherited method
void UpdatePropertyCommand::acquireHandler() {
    MDSBatchCommand::acquireHandler();
}

// inherited method
void UpdatePropertyCommand::ccHandler() {
    MDSBatchCommand::ccHandler();
    switch(request->phase) {
        case MESSAGE_PHASE_UNSENT: {
            sendMessage(*request,
                        update_property_pack.get());
            BC_END_RUNNING_PROPERTY
        }
            
        case MESSAGE_PHASE_SENT: {
            manageRequestPhase(*request);
            break;
        }
            
        case MESSAGE_PHASE_COMPLETED:
        case MESSAGE_PHASE_TIMEOUT: {
            if(request->request_future->getError()) {
                CU_UP_ERR << "Error:" <<request->request_future->getError()<<"\nError Domain:" << request->request_future->getErrorDomain() << "\nError Message:" << request->request_future->getErrorMessage();
            }
            BC_END_RUNNING_PROPERTY
            break;
            
        default:
            break;
        }
    }
}

// inherited method
bool UpdatePropertyCommand::timeoutHandler() {
    return MDSBatchCommand::timeoutHandler();
}
