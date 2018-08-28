/*
 * Copyright 2012, 24/01/2018 INFN
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

#include <chaos_metadata_service_client/api_proxy/agent/GetAgentForNode.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::data::structured;
using namespace chaos::service_common::data::agent;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::agent;

API_PROXY_CD_DEFINITION(GetAgentForNode,
                        AgentNodeDomainAndActionRPC::ProcessWorker::RPC_DOMAIN,
                        "getAgentForNode");

ApiProxyResult GetAgentForNode::execute(const std::string& node_uid) {
    CDWUniquePtr pack(new CDataWrapper());
    pack->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, node_uid);
    return callApi(pack);
}

std::string GetAgentForNode::deserialize(CDataWrapper& api_result) {
    int err_code = CDW_GET_INT32_WITH_DEFAULT(&api_result, RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE, 0);
    std::string err_message = CDW_GET_SRT_WITH_DEFAULT(&api_result, RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_MESSAGE, "");
    std::string err_domain = CDW_GET_SRT_WITH_DEFAULT(&api_result, RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_DOMAIN, "");
    if(err_code) {
        throw CException(err_code,
                         err_message,
                         err_domain);
    }
    std::string result = CDW_GET_SRT_WITH_DEFAULT(&api_result, NodeDefinitionKey::NODE_UNIQUE_ID, "");
    return result;
}
