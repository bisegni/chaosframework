/*
 * Copyright 2012, 02/02/2018 INFN
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
#include "KillCurrentCommand.h"

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::batch_command;
using namespace chaos::metadata_service::api::node;
using namespace chaos::metadata_service::persistence::data_access;

#define INFO INFO_LOG(CommandInstanceSubmit)
#define DBG  DBG_LOG(CommandInstanceSubmit)
#define ERR  ERR_LOG(CommandInstanceSubmit)

KillCurrentCommand::KillCurrentCommand():
ForwardNodeRpcMessage("killCurrentCommand"){}

KillCurrentCommand::~KillCurrentCommand() {}

CDataWrapper *KillCurrentCommand::execute(CDataWrapper *api_data,
                                          bool& detach_data) throw(chaos::CException) {
    CDWUniquePtr node_description;
    CHECK_CDW_THROW_AND_LOG(api_data, ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(api_data, NodeDefinitionKey::NODE_UNIQUE_ID, ERR, -2, CHAOS_FORMAT("The attribute %1% is mandatory",%NodeDefinitionKey::NODE_UNIQUE_ID));
    CHAOS_LASSERT_EXCEPTION(api_data->isStringValue(NodeDefinitionKey::NODE_UNIQUE_ID), ERR, -3, CHAOS_FORMAT("The attribute %1% need to be string",%NodeDefinitionKey::NODE_UNIQUE_ID));
    
    //complete data with domain and action name
    api_data->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_NAME,
                             BatchCommandExecutorRpcActionKey::RPC_KILL_CURRENT_COMMAND);
    return ForwardNodeRpcMessage::execute(api_data,
                                          detach_data);
}
