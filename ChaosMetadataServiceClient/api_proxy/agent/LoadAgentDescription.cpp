/*
 *	LoadAgentDescription.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 13/02/2017 INFN, National Institute of Nuclear Physics
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

#include <ChaosMetadataServiceClient/api_proxy/agent/LoadAgentDescription.h>

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
    std::auto_ptr<CDataWrapper> pack(new CDataWrapper());
    pack->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, node_uid);
    pack->addBoolValue("load_related_data", load_related_data);
    return callApi(pack.release());
}

void LoadAgentDescription::deserialize(CDataWrapper *api_result,
                                   AgentInstance& agent_instance) {
    AgentInstanceSDWrapper agent_instance_sd_wrapper(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(AgentInstance, agent_instance));
    agent_instance_sd_wrapper.deserialize(api_result);
}
