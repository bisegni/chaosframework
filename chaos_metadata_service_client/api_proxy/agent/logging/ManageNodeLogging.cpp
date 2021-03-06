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

#include <chaos_metadata_service_client/api_proxy/agent/logging/ManageNodeLogging.h>

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
    CDWUniquePtr pack(new CDataWrapper());
    pack->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, node_uid);
    pack->addInt32Value("NodeAssociationLoggingOperation", logging_operation);
    return callApi(pack);
}
