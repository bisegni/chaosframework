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

#include <ChaosMetadataServiceClient/api_proxy/agent/LoadNodeAssociation.h>

#include <chaos/common/data/structured/Lists.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::service_common::data::agent;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::agent;

API_PROXY_CD_DEFINITION(LoadNodeAssociation,
                        AgentNodeDomainAndActionRPC::ProcessWorker::RPC_DOMAIN,
                        "loadNodeAssociation");

ApiProxyResult LoadNodeAssociation::execute(const std::string& agent_uid,
                                            const std::string& associated_node_uid) {
    ChaosUniquePtr<chaos::common::data::CDataWrapper> pack(new CDataWrapper());
    pack->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, agent_uid);
    pack->addStringValue(AgentNodeDefinitionKey::NODE_ASSOCIATED, associated_node_uid);
    return callApi(pack.release());
}

void LoadNodeAssociation::deserialize(CDataWrapper *api_result,
                                      AgentAssociation& node_association) {
    AgentAssociationSDWrapper assoc_sd_wrapper(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(AgentAssociation, node_association));
    assoc_sd_wrapper.deserialize(api_result);
}
