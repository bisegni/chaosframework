/*
 *	EndUploadSession.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 02/03/2017 INFN, National Institute of Nuclear Physics
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

#include "EndUploadSession.h"
#include "../../batch/agent/AgentCheckAgentProcess.h"

#include <chaos_service_common/data/data.h>

using namespace chaos::metadata_service::api::deploy;

#define INFO INFO_LOG(EndUploadSession)
#define ERR  DBG_LOG(EndUploadSession)
#define DBG  ERR_LOG(EndUploadSession)

using namespace chaos::common::data;
using namespace chaos::common::data::structured;
using namespace chaos::service_common::data::agent;
using namespace chaos::metadata_service::api::deploy;
using namespace chaos::metadata_service::persistence::data_access;

EndUploadSession::EndUploadSession():
AbstractApi(AgentNodeDomainAndActionRPC::DeployWorker::ACTION_END_DEPLOY_SESSION){
}

EndUploadSession::~EndUploadSession() {
}

CDataWrapper *EndUploadSession::execute(CDataWrapper *api_data, bool& detach_data) {
    //check for mandatory attributes
    return NULL;
}
