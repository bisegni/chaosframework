/*
 *	RemoveNodeAssociation.cpp
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

#include <ChaosMetadataServiceClient/api_proxy/agent/RemoveNodeAssociation.h>

#include <chaos/common/data/structured/Lists.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::service_common::data::agent;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::agent;

API_PROXY_CD_DEFINITION(RemoveNodeAssociation,
                        AgentNodeDomainAndActionRPC::RPC_DOMAIN,
                        "removeNodeAssociation");

ApiProxyResult RemoveNodeAssociation::execute(const std::string& agent_uid,
                                              const std::string& associated_node_uid) {
    std::auto_ptr<CDataWrapper> pack(new CDataWrapper());
    pack->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, agent_uid);
    pack->addStringValue(AgentNodeDefinitionKey::NODE_ASSOCIATED, associated_node_uid);
    return callApi(pack.release());
}
