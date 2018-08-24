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

#include <chaos_metadata_service_client/api_proxy/agent/logging/GetProcessLogEntries.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::data::structured;
using namespace chaos::service_common::data::agent;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::agent::logging;

API_PROXY_CD_DEFINITION(GetProcessLogEntries,
                        AgentNodeDomainAndActionRPC::LogWorker::RPC_DOMAIN,
                        "getProcessLogEntries");

ApiProxyResult GetProcessLogEntries::execute(const std::string& node_uid,
                                             const int32_t number_of_entries,
                                             const bool asc,
                                             const uint64_t start_seq) {
    ChaosUniquePtr<chaos::common::data::CDataWrapper> api_data(new CDataWrapper());
    api_data->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, node_uid);
    api_data->addInt32Value("number_of_entries", number_of_entries);
    api_data->addBoolValue("asc_ordered", asc);
    api_data->addInt64Value("start_seq", start_seq);
    return callApi(api_data);
}

void GetProcessLogEntries::deserialize(CDataWrapper *api_result,
                                      chaos::service_common::data::agent::VectorAgentLogEntry& log_entries_vec) {
    VectorAgentLogEntrySDWrapper lev_w(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(VectorAgentLogEntry, log_entries_vec));
    lev_w.serialization_key = "log_entries";
    lev_w.deserialize(api_result);
}
