/*
 *	DeployWorker.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 01/03/2017 INFN, National Institute of Nuclear Physics
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

#include "DeployWorker.h"
#include "../ChaosAgent.h"

#include <chaos/common/chaos_constants.h>
#include <chaos/common/utility/UUIDUtil.h>

#include <boost/filesystem.hpp>


#define INFO  INFO_LOG(DeployWorker)
#define ERROR ERR_LOG(DeployWorker)
#define DBG   DBG_LOG(DeployWorker)

using namespace chaos::agent;
using namespace chaos::agent::worker;
using namespace chaos::service_common::data::agent;

using namespace chaos::common::data;
using namespace chaos::common::utility;

DeployWorker::DeployWorker():
AbstractWorker(AgentNodeDomainAndActionRPC::DeployWorker::RPC_DOMAIN) {
    //register rpc action
    DeclareAction::addActionDescritionInstance<DeployWorker>(this,
                                                             &DeployWorker::initDeploySession,
                                                             getName(),
                                                             AgentNodeDomainAndActionRPC::DeployWorker::ACTION_INIT_DEPLOY_SESSION,
                                                             "Initialize a deploy session");
    DeclareAction::addActionDescritionInstance<DeployWorker>(this,
                                                             &DeployWorker::uploadDeployChunk,
                                                             getName(),
                                                             AgentNodeDomainAndActionRPC::DeployWorker::ACTION_UPLOAD_DEPLOY_CHUNK,
                                                             "Manage a new chunk foa a deploy session");
    DeclareAction::addActionDescritionInstance<DeployWorker>(this,
                                                             &DeployWorker::endDeploySession,
                                                             getName(),
                                                             AgentNodeDomainAndActionRPC::DeployWorker::ACTION_END_DEPLOY_SESSION,
                                                             "End a deploy session");
}

DeployWorker::~DeployWorker() {}

void DeployWorker::init(void *data) throw(chaos::CException) {}

void DeployWorker::deinit() throw(chaos::CException) {}

CDataWrapper *DeployWorker::initDeploySession(CDataWrapper *data,
                                              bool& detach) {
    CHECK_CDW_THROW_AND_LOG(data,
                            ERROR, -1,
                            "Init session without data si not possible");
    
    CHECK_MANDATORY_KEY(data, AgentNodeDomainAndActionRPC::DeployWorker::ACTION_PARAM_SESSION_ID, ERROR, -2);
    CHECK_MANDATORY_KEY(data, AgentNodeDomainAndActionRPC::DeployWorker::ACTION_PARAM_CHUNK_NUMBER, ERROR, -3);
    
    CHECK_TYPE_OF_KEY(data, AgentNodeDomainAndActionRPC::DeployWorker::ACTION_PARAM_SESSION_ID, String, ERROR, -4);
    CHECK_TYPE_OF_KEY(data, AgentNodeDomainAndActionRPC::DeployWorker::ACTION_PARAM_CHUNK_NUMBER, Int32, ERROR, -5);

    return NULL;
}

CDataWrapper *DeployWorker::uploadDeployChunk(CDataWrapper *data,
                                              bool& detach) {
    CHECK_CDW_THROW_AND_LOG(data,
                            ERROR, -1,
                            "Init session without data si not possible");
    
    CHECK_MANDATORY_KEY(data, AgentNodeDomainAndActionRPC::DeployWorker::ACTION_PARAM_SESSION_ID, ERROR, -2);
    CHECK_MANDATORY_KEY(data, AgentNodeDomainAndActionRPC::DeployWorker::ACTION_PARAM_CHUNK_NUMBER, ERROR, -3);
    CHECK_MANDATORY_KEY(data, AgentNodeDomainAndActionRPC::DeployWorker::ACTION_PARAM_CHUNK_OFFSET, ERROR, -4);
    CHECK_MANDATORY_KEY(data, AgentNodeDomainAndActionRPC::DeployWorker::ACTION_PARAM_CHUNK_DATA, ERROR, -5);
    
    CHECK_TYPE_OF_KEY(data, AgentNodeDomainAndActionRPC::DeployWorker::ACTION_PARAM_SESSION_ID, String, ERROR, -6);
    CHECK_TYPE_OF_KEY(data, AgentNodeDomainAndActionRPC::DeployWorker::ACTION_PARAM_CHUNK_NUMBER, Int32, ERROR, -7);
    CHECK_TYPE_OF_KEY(data, AgentNodeDomainAndActionRPC::DeployWorker::ACTION_PARAM_CHUNK_OFFSET, Int32, ERROR, -8);
    CHECK_TYPE_OF_KEY(data, AgentNodeDomainAndActionRPC::DeployWorker::ACTION_PARAM_CHUNK_DATA, Binary, ERROR, -9);

    return NULL;
}

CDataWrapper *DeployWorker::endDeploySession(CDataWrapper *data,
                                             bool& detach) {
    CHECK_CDW_THROW_AND_LOG(data,
                            ERROR, -1,
                            "Init session without data si not possible");
    
    CHECK_MANDATORY_KEY(data, AgentNodeDomainAndActionRPC::DeployWorker::ACTION_PARAM_SESSION_ID, ERROR, -2);
    CHECK_MANDATORY_KEY(data, AgentNodeDomainAndActionRPC::DeployWorker::ACTION_PARAM_SESSION_HASH, ERROR, -3);
    
    CHECK_TYPE_OF_KEY(data, AgentNodeDomainAndActionRPC::DeployWorker::ACTION_PARAM_SESSION_ID, String, ERROR, -4);
    CHECK_TYPE_OF_KEY(data, AgentNodeDomainAndActionRPC::DeployWorker::ACTION_PARAM_SESSION_HASH, String, ERROR, -5);
    return NULL;
}
