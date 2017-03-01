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
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

#define INFO  INFO_LOG(DeployWorker)
#define ERROR ERR_LOG(DeployWorker)
#define DBG   DBG_LOG(DeployWorker)

using namespace chaos::agent;
using namespace chaos::agent::worker;
using namespace chaos::service_common::data::agent;

using namespace chaos::common::data;
using namespace chaos::common::utility;

DeployWorker::DeployWorker():
AbstractWorker(AgentNodeDomainAndActionRPC::DeployWorker::WORKER_NAME) {
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

DeployWorker::~DeployWorker() {
    
}


void DeployWorker::init(void *data) throw(chaos::CException) {
    
}

void DeployWorker::deinit() throw(chaos::CException) {}


CDataWrapper *DeployWorker::initDeploySession(CDataWrapper *data,
                                              bool& detach) {
    return NULL;
}

CDataWrapper *DeployWorker::uploadDeployChunk(CDataWrapper *data,
                                              bool& detach) {
    return NULL;
}

CDataWrapper *DeployWorker::endDeploySession(CDataWrapper *data,
                                             bool& detach) {
    return NULL;
}
