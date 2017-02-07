/*
 *	ProcessWorker.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 06/02/2017 INFN, National Institute of Nuclear Physics
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

#include "ProcessWorker.h"

#include <chaos/common/chaos_constants.h>

using namespace chaos::agent::worker;

ProcessWorker::ProcessWorker():
AbstractWorker(AgentNodeDomainAndActionRPC::ProcessWorker::WORKER_NAME) {
    //register rpc action
    AbstActionDescShrPtr action_parameter_interface = DeclareAction::addActionDescritionInstance<ProcessWorker>(this,
                                                                                                                &ProcessWorker::launchUnitServer,
                                                                                                                getName(),
                                                                                                                AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_LAUNCH_UNIT_SERVER,
                                                                                                                "Start an unit server");
    action_parameter_interface->addParam(AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_LAUNCH_UNIT_SERVER_PAR_NAME,
                                         DataType::TYPE_STRING,
                                         "The name of the unit server to launch");
    action_parameter_interface->addParam(AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_LAUNCH_UNIT_SERVER_PAR_CFG,
                                         DataType::TYPE_STRING,
                                         "The content of init file of the unit servers");
    
    
    action_parameter_interface = DeclareAction::addActionDescritionInstance<ProcessWorker>(this,
                                                                                           &ProcessWorker::stopUnitServer,
                                                                                           getName(),
                                                                                           AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_STOP_UNIT_SERVER,
                                                                                           "Stop an unit server");
    action_parameter_interface->addParam(AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_STOP_UNIT_SERVER_PAR_NAME,
                                         DataType::TYPE_STRING,
                                         "The name of the unit server to launch");
    
    action_parameter_interface = DeclareAction::addActionDescritionInstance<ProcessWorker>(this,
                                                                                           &ProcessWorker::restartUnitServer,
                                                                                           getName(),
                                                                                           AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_RESTART_UNIT_SERVER,
                                                                                           "Restart an unit server");
    action_parameter_interface->addParam(AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_RESTART_UNIT_SERVER_PAR_NAME,
                                         DataType::TYPE_STRING,
                                         "The name of the unit server to launch");
    
    action_parameter_interface->addParam(AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_RESTART_UNIT_SERVER_PAR_KILL,
                                         DataType::TYPE_BOOLEAN,
                                         "Kill the process for stop it");
    
    action_parameter_interface = DeclareAction::addActionDescritionInstance<ProcessWorker>(this,
                                                                                           &ProcessWorker::listUnitServers,
                                                                                           getName(),
                                                                                           AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_LIST_UNIT_SERVER,
                                                                                           "List all unit server");
}

ProcessWorker::~ProcessWorker() {
    
}

void ProcessWorker::init(void *data) throw(chaos::CException) {
    
}

void ProcessWorker::deinit() throw(chaos::CException) {}


chaos::common::data::CDataWrapper *ProcessWorker::launchUnitServer(chaos::common::data::CDataWrapper *data,
                                                                    bool& detach) {
    return NULL;
}

chaos::common::data::CDataWrapper *ProcessWorker::stopUnitServer(chaos::common::data::CDataWrapper *data,
                                                                  bool& detach) {
    return NULL;
}

chaos::common::data::CDataWrapper *ProcessWorker::restartUnitServer(chaos::common::data::CDataWrapper *data,
                                                                     bool& detach) {
    return NULL;
}

chaos::common::data::CDataWrapper *ProcessWorker::listUnitServers(chaos::common::data::CDataWrapper *data,
                                                                   bool& detach) {
    return NULL;
}
