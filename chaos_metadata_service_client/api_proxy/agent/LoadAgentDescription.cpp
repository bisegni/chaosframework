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

#include <chaos_metadata_service_client/api_proxy/agent/LoadAgentDescription.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::data::structured;
using namespace chaos::service_common::data::agent;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::agent;

API_PROXY_CD_DEFINITION(LoadAgentDescription,
                        AgentNodeDomainAndActionRPC::ProcessWorker::RPC_DOMAIN,
                        "loadAgentDescription");

ApiProxyResult LoadAgentDescription::execute(const std::string& node_uid,
                                             const bool load_related_data) {
    CDWUniquePtr pack(new CDataWrapper());
    pack->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, node_uid);
    pack->addBoolValue("load_related_data", load_related_data);
    return callApi(pack);
}

void LoadAgentDescription::deserialize(CDataWrapper *api_result,
                                   AgentInstance& agent_instance) {
    AgentInstanceSDWrapper agent_instance_sd_wrapper(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(AgentInstance, agent_instance));
    agent_instance_sd_wrapper.deserialize(api_result);
}
