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

#include "GetProcessLogEntries.h"
#include "../../../batch/agent/AgentLoggingNodeManagement.h"

#include <chaos_service_common/data/data.h>

using namespace chaos::metadata_service::api::agent::logging;

#define INFO INFO_LOG(RemoveNodeAssociation)
#define ERR  DBG_LOG(RemoveNodeAssociation)
#define DBG  ERR_LOG(RemoveNodeAssociation)

using namespace chaos::common::data;
using namespace chaos::common::data::structured;
using namespace chaos::service_common::data::agent;
using namespace chaos::metadata_service::api::agent;
using namespace chaos::metadata_service::persistence::data_access;

GetProcessLogEntries::GetProcessLogEntries():
AbstractApi("getProcessLogEntries"){
}

GetProcessLogEntries::~GetProcessLogEntries() {
}

CDWUniquePtr GetProcessLogEntries::execute(CDWUniquePtr api_data) {
    //check for mandatory attributes
    CHECK_CDW_THROW_AND_LOG(api_data, ERR, -1, "No parameter found");
    CHECK_MANDATORY_KEY(api_data, NodeDefinitionKey::NODE_UNIQUE_ID, ERR, -2);
    CHECK_TYPE_OF_KEY(api_data, NodeDefinitionKey::NODE_UNIQUE_ID, String, ERR, -3);
    
    CHECK_MANDATORY_KEY(api_data, "number_of_entries", ERR, -4);
    CHECK_TYPE_OF_KEY(api_data, "number_of_entries", Int32, ERR, -5);
    
    CHECK_MANDATORY_KEY(api_data, "asc_ordered", ERR, -6);
    CHECK_TYPE_OF_KEY(api_data, "asc_ordered", Bool, ERR, -7);
    
    CHECK_MANDATORY_KEY(api_data, "start_seq", ERR, -8);
    CHECK_TYPE_OF_KEY(api_data, "start_seq", Int64, ERR, -9);
    
    //we can rpocessd
    int err = 0;
    GET_DATA_ACCESS(AgentDataAccess, a_da, -10);
    const std::string node_uid = api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    const int32_t number_of_entries = api_data->getInt32Value("number_of_entries");
    const bool asc_ordered = api_data->getBoolValue("asc_ordered");
    const uint64_t start_seq = api_data->getUInt64Value("start_seq");
    
    VectorAgentLogEntrySDWrapper lev_w;
    lev_w.serialization_key = "log_entries";
    if((err = a_da->getLogEntry(node_uid,
                                number_of_entries,
                                asc_ordered,
                                start_seq,
                                lev_w()))){
        LOG_AND_TROW(ERR, -11, CHAOS_FORMAT("Error retrieving log entries for node %1%", %node_uid));
    }
    return lev_w.serialize().release();
}
