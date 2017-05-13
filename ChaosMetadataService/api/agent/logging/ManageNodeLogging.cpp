/*
 *	ManageNodeLogging.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 07/03/2017 INFN, National Institute of Nuclear Physics
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

#include "ManageNodeLogging.h"
#include "../../../batch/agent/AgentLoggingNodeManagement.h"

#include <chaos_service_common/data/data.h>

using namespace chaos::metadata_service::api::agent::logging;

#define INFO INFO_LOG(RemoveNodeAssociation)
#define ERR  DBG_LOG(RemoveNodeAssociation)
#define DBG  ERR_LOG(RemoveNodeAssociation)

using namespace chaos::common::data;
using namespace chaos::common::data::structured;
using namespace chaos::service_common::data::agent;
using namespace chaos::metadata_service::api::agent;
using namespace chaos::metadata_service::persistence::data_access;

ManageNodeLogging::ManageNodeLogging():
AbstractApi("manageNodeLogging"){
}

ManageNodeLogging::~ManageNodeLogging() {
}

CDataWrapper *ManageNodeLogging::execute(CDataWrapper *api_data, bool& detach_data) {
    //check for mandatory attributes
    CHECK_CDW_THROW_AND_LOG(api_data, ERR, -1, "No parameter found");
    CHECK_MANDATORY_KEY(api_data, NodeDefinitionKey::NODE_UNIQUE_ID, ERR, -2);
    CHECK_TYPE_OF_KEY(api_data, NodeDefinitionKey::NODE_UNIQUE_ID, String, ERR, -3);
    
    CHECK_MANDATORY_KEY(api_data, "NodeAssociationLoggingOperation", ERR, -4);
    CHECK_TYPE_OF_KEY(api_data, "NodeAssociationLoggingOperation", Int32, ERR, -5);
    
    //we can rpocessd
    GET_DATA_ACCESS(AgentDataAccess, a_da, -4);
    uint64_t cmd_id = 0;
    std::unique_ptr<CDataWrapper> batch_data(new CDataWrapper());
    api_data->copyKeyTo(NodeDefinitionKey::NODE_UNIQUE_ID, *batch_data);
    api_data->copyKeyTo("NodeAssociationLoggingOperation", *batch_data);
    cmd_id = getBatchExecutor()->submitCommand(GET_MDS_COMMAND_ALIAS(batch::agent::AgentLoggingNodeManagement),
                                               batch_data.release(),
                                               0,
                                               1000);
    return NULL;
}
