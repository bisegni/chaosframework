/*
 *	GetProcessLogEntries.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 09/03/2017 INFN, National Institute of Nuclear Physics
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

#include <ChaosMetadataServiceClient/api_proxy/agent/logging/GetProcessLogEntries.h>

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
                                             const uint64_t start_ts) {
    std::auto_ptr<CDataWrapper> api_data(new CDataWrapper());
    api_data->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, node_uid);
    api_data->addInt32Value("number_of_entries", number_of_entries);
    api_data->addBoolValue("asc_ordered", asc);
    api_data->addInt64Value("start_ts", start_ts);
    return callApi(api_data.release());
}

void GetProcessLogEntries::deserialize(CDataWrapper *api_result,
                                      chaos::service_common::data::agent::VectorAgentLogEntry& log_entries_vec) {
    VectorAgentLogEntrySDWrapper lev_w(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(VectorAgentLogEntry, log_entries_vec));
    lev_w.deserialize(api_result);
}
