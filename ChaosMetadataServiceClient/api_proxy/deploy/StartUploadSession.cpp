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

#include <ChaosMetadataServiceClient/api_proxy/deploy/StartUploadSession.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::data::structured;
using namespace chaos::service_common::data::agent;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::deploy;

API_PROXY_CD_DEFINITION(StartUploadSession,
                        AgentNodeDomainAndActionRPC::DeployWorker::RPC_DOMAIN,
                        AgentNodeDomainAndActionRPC::DeployWorker::ACTION_INIT_DEPLOY_SESSION);

ApiProxyResult StartUploadSession::execute(const std::string& agent_uid) {
    ChaosUniquePtr<chaos::common::data::CDataWrapper> pack(new CDataWrapper());
    return callApi(pack.release());
}
