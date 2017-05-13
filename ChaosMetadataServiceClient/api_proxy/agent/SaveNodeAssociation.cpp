/*
 *	SaveNodeAssociation.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 09/02/2017 INFN, National Institute of Nuclear Physics
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

#include <ChaosMetadataServiceClient/api_proxy/agent/SaveNodeAssociation.h>

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
    std::unique_ptr<CDataWrapper> pack(new CDataWrapper());
    std::unique_ptr<CDataWrapper> assoc_pack = assoc_sd_wrapper.serialize();
    
    pack->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, agent_uid);
    pack->addCSDataValue(AgentNodeDefinitionKey::NODE_ASSOCIATED, *assoc_pack);
    return callApi(pack.release());
}

ApiProxyResult SaveNodeAssociation::execute(const std::string& agent_uid,
		chaos::common::data::CDataWrapper& node_association) {
    std::unique_ptr<CDataWrapper> pack(new CDataWrapper());

    pack->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, agent_uid);
    pack->addCSDataValue(AgentNodeDefinitionKey::NODE_ASSOCIATED, node_association);
    return callApi(pack.release());
}

ApiProxyResult SaveNodeAssociation::execute(const std::string& agent_uid,
                                            VectorAgentAssociation& multiple_node_association) {
    std::unique_ptr<CDataWrapper> pack(new CDataWrapper());
    VectorAgentAssociationSDWrapper associationList_sd_wrap(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(VectorAgentAssociation, multiple_node_association));
    associationList_sd_wrap.serialization_key = AgentNodeDefinitionKey::NODE_ASSOCIATED;
    std::unique_ptr<CDataWrapper> multiple_assoc_pack = associationList_sd_wrap.serialize();
    pack->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, agent_uid);
    pack->appendAllElement(*multiple_assoc_pack);
    return callApi(pack.release());
}

