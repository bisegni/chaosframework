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
AbstractApi("killCurrentCommand"){
    
}

KillCurrentCommand::~KillCurrentCommand() {
    
}

CDataWrapper *KillCurrentCommand::execute(CDataWrapper *api_data,
                                          bool& detach_data) throw(chaos::CException) {
    int err = 0;
    CHECK_CDW_THROW_AND_LOG(api_data, ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(api_data, NodeDefinitionKey::NODE_UNIQUE_ID, ERR, -2, CHAOS_FORMAT("The attribute %1% is mandatory",%NodeDefinitionKey::NODE_UNIQUE_ID));
    CHAOS_LASSERT_EXCEPTION(api_data->isStringValue(NodeDefinitionKey::NODE_UNIQUE_ID), ERR, -3, CHAOS_FORMAT("The attribute %1% need to be string",%NodeDefinitionKey::NODE_UNIQUE_ID));
    GET_DATA_ACCESS(NodeDataAccess, n_da, -4);
    CDWUniquePtr description;
    const std::string node_uid = api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    
    if((err = n_da->getNodeDescription(node_uid, description))) {
        LOG_AND_TROW(ERR, -5, CHAOS_FORMAT("Error fetching description for node %1%",%NodeDefinitionKey::NODE_UNIQUE_ID));
    } else {
        //send batch command to perform forwarding to control node for execute the action
    }
    
    return NULL;
}
