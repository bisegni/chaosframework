/*
 *	GetManagementConfiguration.cpp
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

#include "GetManagementConfiguration.h"
#include "../../batch/agent/AgentCheckAgentProcess.h"

#include <chaos_service_common/data/data.h>

using namespace chaos::metadata_service::api::agent;

#define INFO INFO_LOG(GetManagementConfiguration)
#define ERR  DBG_LOG(GetManagementConfiguration)
#define DBG  ERR_LOG(GetManagementConfiguration)

using namespace chaos::common::data;
using namespace chaos::common::data::structured;
using namespace chaos::service_common::data::agent;
using namespace chaos::metadata_service::api::agent;
using namespace chaos::metadata_service::persistence::data_access;

GetManagementConfiguration::GetManagementConfiguration():
AbstractApi("getManagementConfiguration"){}

GetManagementConfiguration::~GetManagementConfiguration(){}

CDataWrapper *GetManagementConfiguration::execute(CDataWrapper *api_data, bool& detach_data) {
    GET_DATA_ACCESS(AgentDataAccess, a_da, -1);
    AgentManagementSettingSDWrapper set_w;
    a_da->getLogEntryExpiration(set_w().expiration_enabled, set_w().log_expiration_in_seconds);
    return set_w.serialize().release();
}
