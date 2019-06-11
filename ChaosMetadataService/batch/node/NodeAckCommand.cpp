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

#include "NodeAckCommand.h"

#include "../../common/CUCommonUtility.h"

using namespace chaos::common::data;
using namespace chaos::common::network;


using namespace chaos::metadata_service::common;
using namespace chaos::metadata_service::batch;
using namespace chaos::metadata_service::batch::node;

#define INFO INFO_LOG(NodeAckCommand)
#define DBG  DBG_LOG(NodeAckCommand)
#define ERR  ERR_LOG(NodeAckCommand)

DEFINE_MDS_COMAMND_ALIAS(NodeAckCommand)

NodeAckCommand::NodeAckCommand():
MDSBatchCommand(){}

NodeAckCommand::~NodeAckCommand() {}

// inherited method
void NodeAckCommand::setHandler(CDataWrapper *data) {
    MDSBatchCommand::setHandler(data);
    CHECK_ASSERTION_THROW_AND_LOG((data!= NULL), ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(data, chaos::NodeDefinitionKey::NODE_UNIQUE_ID, ERR, -2, "The unique id of unit server is mandatory")
    CHECK_KEY_THROW_AND_LOG(data, chaos::NodeDefinitionKey::NODE_RPC_ADDR, ERR, -3, "The rpc address of unit server is mandatory")
    
    int err = 0;
    
    node_uid = data->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID);

    
    request = createRequest(data->getStringValue(chaos::NodeDefinitionKey::NODE_RPC_ADDR),
                            NodeDomainAndActionRPC::RPC_DOMAIN,
                            NodeDomainAndActionRPC::ACTION_REGISTRATION_ACK);
    message_data.reset(new CDataWrapper(data->getBSONRawData()));
}

// inherited method
void NodeAckCommand::acquireHandler() {
    
}

// inherited method
void NodeAckCommand::ccHandler() {
    switch(request->phase) {
        case MESSAGE_PHASE_UNSENT: {
            sendMessage(*request,
                        MOVE(message_data));
        }
            
        case MESSAGE_PHASE_SENT:
        case MESSAGE_PHASE_COMPLETED:
        case MESSAGE_PHASE_TIMEOUT:
        default:
            BC_END_RUNNING_PROPERTY
            break;
    }
}

// inherited method
bool NodeAckCommand::timeoutHandler() {
    bool result = MDSBatchCommand::timeoutHandler();
    return result;
}
