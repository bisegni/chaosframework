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
    
    CHECK_ASSERTION_THROW_AND_LOG(data!=NULL, CU_UP_ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(data, NodeDefinitionKey::NODE_UNIQUE_ID, CU_UP_ERR, -2, "The attribute node unique id is mandatory")
    CHECK_KEY_THROW_AND_LOG(data, "property", CU_UP_ERR, -3, "The attribute node unique id is mandatory")

    update_property_pack.reset(new CDataWrapper(data->getBSONRawData()));
    
    const std::string node_uid = data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    //fetch data for sending message to node id
    
    if((err = getDataAccess<mds_data_access::NodeDataAccess>()->getNodeDescription(node_uid, &tmp_ptr))) {
        LOG_AND_TROW_FORMATTED(CU_UP_ERR, err, "Error loading infomation for node '%1%'", %node_uid)
    } else if(!tmp_ptr) {
        LOG_AND_TROW_FORMATTED(CU_UP_ERR, -1, "No description found for node '%1%'", %node_uid)
    }
    
    ChaosUniquePtr<chaos::common::data::CDataWrapper> node_description(tmp_ptr);
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
                        MOVE(update_property_pack));
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
