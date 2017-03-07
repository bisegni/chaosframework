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

#include <ChaosMetadataServiceClient/api_proxy/agent/logging/ManageNodeLogging.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::data::structured;
using namespace chaos::service_common::data::agent;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::agent::logging;

API_PROXY_CD_DEFINITION(ManageNodeLogging,
                        AgentNodeDomainAndActionRPC::LogWorker::RPC_DOMAIN,
                        "manageNodeLogging");

ApiProxyResult ManageNodeLogging::execute(const std::string& node_uid,
                                          const NodeAssociationLoggingOperation logging_operation) {
    std::auto_ptr<CDataWrapper> pack(new CDataWrapper());
    pack->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, node_uid);
    pack->addInt32Value("NodeAssociationLoggingOperation", logging_operation);
    return callApi(pack.release());
}
