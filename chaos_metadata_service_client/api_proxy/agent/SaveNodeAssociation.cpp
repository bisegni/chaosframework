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

#include <chaos_metadata_service_client/api_proxy/agent/SaveNodeAssociation.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::service_common::data::agent;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::agent;

API_PROXY_CD_DEFINITION(SaveNodeAssociation,
                        AgentNodeDomainAndActionRPC::ProcessWorker::RPC_DOMAIN,
                        "saveNodeAssociation");

ApiProxyResult SaveNodeAssociation::execute(const std::string& agent_uid,
                                            AgentAssociation& node_association) {
    AgentAssociationSDWrapper assoc_sd_wrapper(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(AgentAssociation, node_association));
    ChaosUniquePtr<chaos::common::data::CDataWrapper> pack(new CDataWrapper());
    ChaosUniquePtr<chaos::common::data::CDataWrapper> assoc_pack = assoc_sd_wrapper.serialize();
    
    pack->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, agent_uid);
    pack->addCSDataValue(AgentNodeDefinitionKey::NODE_ASSOCIATED, *assoc_pack);
    return callApi(pack.release());
}

ApiProxyResult SaveNodeAssociation::execute(const std::string& agent_uid,
		chaos::common::data::CDataWrapper& node_association) {
    ChaosUniquePtr<chaos::common::data::CDataWrapper> pack(new CDataWrapper());

    pack->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, agent_uid);
    pack->addCSDataValue(AgentNodeDefinitionKey::NODE_ASSOCIATED, node_association);
    return callApi(pack.release());
}

ApiProxyResult SaveNodeAssociation::execute(const std::string& agent_uid,
                                            VectorAgentAssociation& multiple_node_association) {
    ChaosUniquePtr<chaos::common::data::CDataWrapper> pack(new CDataWrapper());
    VectorAgentAssociationSDWrapper associationList_sd_wrap(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(VectorAgentAssociation, multiple_node_association));
    associationList_sd_wrap.serialization_key = AgentNodeDefinitionKey::NODE_ASSOCIATED;
    ChaosUniquePtr<chaos::common::data::CDataWrapper> multiple_assoc_pack = associationList_sd_wrap.serialize();
    pack->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, agent_uid);
    pack->appendAllElement(*multiple_assoc_pack);
    return callApi(pack.release());
}

