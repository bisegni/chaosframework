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

void DeployWorker::init(void *data)  {}

void DeployWorker::deinit()  {}

CDWUniquePtr DeployWorker::initDeploySession(CDWUniquePtr data) {
    CHECK_ASSERTION_THROW_AND_LOG(data.get()!=NULL,
                                  ERROR, -1,
                                  "Init session without data si not possible");
    
    CHECK_MANDATORY_KEY(data, AgentNodeDomainAndActionRPC::DeployWorker::ACTION_PARAM_SESSION_ID, ERROR, -2);
    CHECK_MANDATORY_KEY(data, AgentNodeDomainAndActionRPC::DeployWorker::ACTION_PARAM_CHUNK_NUMBER, ERROR, -3);
    
    CHECK_TYPE_OF_KEY(data, AgentNodeDomainAndActionRPC::DeployWorker::ACTION_PARAM_SESSION_ID, String, ERROR, -4);
    CHECK_TYPE_OF_KEY(data, AgentNodeDomainAndActionRPC::DeployWorker::ACTION_PARAM_CHUNK_NUMBER, Int32, ERROR, -5);
    
    return CDWUniquePtr();
}

CDWUniquePtr DeployWorker::uploadDeployChunk(CDWUniquePtr data) {
    CHECK_ASSERTION_THROW_AND_LOG(data.get()!=NULL,
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
    
    return CDWUniquePtr();
}

CDWUniquePtr DeployWorker::endDeploySession(CDWUniquePtr data) {
    CHECK_ASSERTION_THROW_AND_LOG(data.get()!=NULL,
                                  ERROR, -1,
                                  "Init session without data si not possible");
    
    CHECK_MANDATORY_KEY(data, AgentNodeDomainAndActionRPC::DeployWorker::ACTION_PARAM_SESSION_ID, ERROR, -2);
    CHECK_MANDATORY_KEY(data, AgentNodeDomainAndActionRPC::DeployWorker::ACTION_PARAM_SESSION_HASH, ERROR, -3);
    
    CHECK_TYPE_OF_KEY(data, AgentNodeDomainAndActionRPC::DeployWorker::ACTION_PARAM_SESSION_ID, String, ERROR, -4);
    CHECK_TYPE_OF_KEY(data, AgentNodeDomainAndActionRPC::DeployWorker::ACTION_PARAM_SESSION_HASH, String, ERROR, -5);
    return CDWUniquePtr();
}
