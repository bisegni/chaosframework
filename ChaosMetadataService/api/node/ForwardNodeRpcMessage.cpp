/*
 * Copyright 2012, 05/02/2018 INFN
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

#include "ForwardNodeRpcMessage.h"
#include "../../batch/node/SendRpcCommand.h"

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::batch_command;
using namespace chaos::metadata_service::api::node;
using namespace chaos::metadata_service::persistence::data_access;

#define INFO INFO_LOG(CommandInstanceSubmit)
#define DBG  DBG_LOG(CommandInstanceSubmit)
#define ERR  ERR_LOG(CommandInstanceSubmit)

ForwardNodeRpcMessage::ForwardNodeRpcMessage():
AbstractApi("forwardNodeRpcMessage"){}

ForwardNodeRpcMessage::ForwardNodeRpcMessage(const std::string& superclass_api_name):
AbstractApi(superclass_api_name){}

ForwardNodeRpcMessage::~ForwardNodeRpcMessage() {}

CDWUniquePtr ForwardNodeRpcMessage::execute(CDWUniquePtr api_data) {
    int err = 0;
    uint64_t command_id = 0;
    CHECK_CDW_THROW_AND_LOG(api_data, ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(api_data, NodeDefinitionKey::NODE_UNIQUE_ID, ERR, -2, CHAOS_FORMAT("The attribute %1% is mandatory",%NodeDefinitionKey::NODE_UNIQUE_ID));
    CHAOS_LASSERT_EXCEPTION(api_data->isStringValue(NodeDefinitionKey::NODE_UNIQUE_ID), ERR, -3, CHAOS_FORMAT("The attribute %1% need to be string",%NodeDefinitionKey::NODE_UNIQUE_ID));
    
    CHECK_KEY_THROW_AND_LOG(api_data, RpcActionDefinitionKey::CS_CMDM_ACTION_NAME, ERR, -6, CHAOS_FORMAT("The attribute %1% is mandatory",%RpcActionDefinitionKey::CS_CMDM_ACTION_DOMAIN));
    CHAOS_LASSERT_EXCEPTION(api_data->isStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_NAME), ERR, -7, CHAOS_FORMAT("The attribute %1% need to be string",%RpcActionDefinitionKey::CS_CMDM_ACTION_DOMAIN));
    
    GET_DATA_ACCESS(NodeDataAccess, n_da, -8);
    bool alive;
    const std::string node_uid = api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    
    if((err = n_da->isNodeAlive(node_uid, alive))) {
        LOG_AND_TROW(ERR, -9, CHAOS_FORMAT("Error fetching the alive state for node %1% with code %2%",%node_uid%err));
    } else if(alive == false){
        LOG_AND_TROW(ERR, -10, CHAOS_FORMAT("The %1% is not alive",%node_uid));
    } else {
        command_id = getBatchExecutor()->submitCommand(GET_MDS_COMMAND_ALIAS(batch::node::SendRpcCommand),
                                                       api_data.release());
        INFO << CHAOS_FORMAT("SendRpcCommand submitted with id %1%", %command_id);
    }
    return CDWUniquePtr();
}
