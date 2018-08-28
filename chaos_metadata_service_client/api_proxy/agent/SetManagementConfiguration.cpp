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

#include <chaos_metadata_service_client/api_proxy/agent/SetManagementConfiguration.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::data::structured;
using namespace chaos::service_common::data::agent;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::agent;

API_PROXY_CD_DEFINITION(SetManagementConfiguration,
                        AgentNodeDomainAndActionRPC::ProcessWorker::RPC_DOMAIN,
                        "setManagementConfiguration");

ApiProxyResult SetManagementConfiguration::execute(AgentManagementSetting& agent_man_setting) {
    AgentManagementSettingSDWrapper a_set_sdw(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(AgentManagementSetting, agent_man_setting));
    return callApi(a_set_sdw.serialize().release());
}
