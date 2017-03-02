/*
 *	StartUploadSession.cpp
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

#include "StartUploadSession.h"

#include "../../batch/agent/AgentCheckAgentProcess.h"

#include <chaos_service_common/data/data.h>

using namespace chaos::metadata_service::api::deploy;

#define INFO INFO_LOG(StartUploadSession)
#define ERR  DBG_LOG(StartUploadSession)
#define DBG  ERR_LOG(StartUploadSession)

using namespace chaos::common::data;
using namespace chaos::common::data::structured;
using namespace chaos::metadata_service::api::deploy;
using namespace chaos::metadata_service::persistence::data_access;

StartUploadSession::StartUploadSession():
AbstractApi(AgentNodeDomainAndActionRPC::DeployWorker::ACTION_INIT_DEPLOY_SESSION){
}

StartUploadSession::~StartUploadSession() {
}

CDataWrapper *StartUploadSession::execute(CDataWrapper *api_data, bool& detach_data) {
    //check for mandatory attributes
    return NULL;
}
