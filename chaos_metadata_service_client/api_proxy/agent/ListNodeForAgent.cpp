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

#include <chaos_metadata_service_client/api_proxy/agent/ListNodeForAgent.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::data::structured;
using namespace chaos::service_common::data::agent;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::agent;

API_PROXY_CD_DEFINITION(ListNodeForAgent,
                        AgentNodeDomainAndActionRPC::ProcessWorker::RPC_DOMAIN,
                        AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_LIST_NODE);

ApiProxyResult ListNodeForAgent::execute(const std::string& agent_uid) {
    CDWUniquePtr pack(new CDataWrapper());
    pack->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, agent_uid);
    return callApi(pack);
}

void ListNodeForAgent::deserialize(CDataWrapper *api_result,
                                   VectorAgentAssociationStatus& associated_nodes) {
    VectorAgentAssociationStatusSDWrapper association_list_sd_wrapper(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(VectorAgentAssociationStatus, associated_nodes));
    association_list_sd_wrapper.serialization_key = AgentNodeDefinitionKey::NODE_ASSOCIATED;
    association_list_sd_wrapper.deserialize(api_result);
}
