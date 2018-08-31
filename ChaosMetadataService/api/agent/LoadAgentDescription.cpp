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

#include "LoadAgentDescription.h"

using namespace chaos::metadata_service::api::agent;

#define INFO INFO_LOG(LoadAgentDescription)
#define ERR  DBG_LOG(LoadAgentDescription)
#define DBG  ERR_LOG(LoadAgentDescription)

using namespace chaos::common::data;
using namespace chaos::service_common::data::agent;
using namespace chaos::metadata_service::api::agent;
using namespace chaos::metadata_service::persistence::data_access;

LoadAgentDescription::LoadAgentDescription():
AbstractApi("loadAgentDescription"){}

LoadAgentDescription::~LoadAgentDescription(){}

CDWUniquePtr LoadAgentDescription::execute(CDWUniquePtr api_data) {
    //check for mandatory attributes
    CHECK_CDW_THROW_AND_LOG(api_data, ERR, -1, "No parameter found");
    CHECK_KEY_THROW_AND_LOG(api_data, NodeDefinitionKey::NODE_UNIQUE_ID, ERR, -2, CHAOS_FORMAT("The key %1% is mandatory", %NodeDefinitionKey::NODE_UNIQUE_ID));
    CHAOS_LASSERT_EXCEPTION(api_data->isStringValue(NodeDefinitionKey::NODE_UNIQUE_ID), ERR, -3, CHAOS_FORMAT("The key %1% need to be a string", %NodeDefinitionKey::NODE_UNIQUE_ID));
    
    //we can rpocessd
    GET_DATA_ACCESS(AgentDataAccess, a_da, -4);
    
    int err = 0;
    bool load_related_data = false;
    AgentInstanceSDWrapper agent_instance_sd_wrapper;
    const std::string agent_uid = api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    if(api_data->hasKey("load_related_data")) {
        load_related_data = api_data->getBoolValue("load_related_data");
    }
    if((err = a_da->loadAgentDescription(agent_uid,
                                         load_related_data,
                                         agent_instance_sd_wrapper()))) {
        LOG_AND_TROW(ERR, -5, CHAOS_FORMAT("Error loading full description for agent %1%",%agent_uid));
    }
    return agent_instance_sd_wrapper.serialize().release();
}
