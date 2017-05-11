/*
 *	SetManagementConfiguration.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 11/05/2017 INFN, National Institute of Nuclear Physics
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

#include "SetManagementConfiguration.h"
#include "../../batch/agent/AgentCheckAgentProcess.h"

#include <chaos_service_common/data/data.h>

using namespace chaos::metadata_service::api::agent;

#define INFO INFO_LOG(SetManagementConfiguration)
#define ERR  DBG_LOG(SetManagementConfiguration)
#define DBG  ERR_LOG(SetManagementConfiguration)

using namespace chaos::common::data;
using namespace chaos::common::data::structured;
using namespace chaos::service_common::data::agent;
using namespace chaos::metadata_service::api::agent;
using namespace chaos::metadata_service::persistence::data_access;

SetManagementConfiguration::SetManagementConfiguration():
AbstractApi("setManagementConfiguration"){}

SetManagementConfiguration::~SetManagementConfiguration(){}

CDataWrapper *SetManagementConfiguration::execute(CDataWrapper *api_data, bool& detach_data) {
    //check for mandatory attributes
    CHECK_CDW_THROW_AND_LOG(api_data, ERR, -1, "No parameter found");
    GET_DATA_ACCESS(AgentDataAccess, a_da, -2);
    AgentManagementSettingSDWrapper set_w(api_data);
    a_da->setLogEntryExpiration(set_w().expiration_enabled, set_w().log_expiration_in_seconds);
    return set_w.serialize().release();
}
