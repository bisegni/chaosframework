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
#include "../ChaosAgent.h"

#include <chaos_service_common/data/node/Agent.h>

#include <chaos/common/chaos_constants.h>

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

#define INFO    INFO_LOG(ProcessWorker)
#define ERROR   ERR_LOG(ProcessWorker)
#define DBG   DBG_LOG(ProcessWorker)

using namespace chaos::agent;
using namespace chaos::agent::worker;
using namespace chaos::service_common::data::agent;

ProcessWorker::ProcessWorker():
AbstractWorker(AgentNodeDomainAndActionRPC::ProcessWorker::WORKER_NAME) {
    //register rpc action
    AbstActionDescShrPtr action_parameter_interface = DeclareAction::addActionDescritionInstance<ProcessWorker>(this,
                                                                                                                &ProcessWorker::launchUnitServer,
                                                                                                                getName(),
                                                                                                                AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_LAUNCH_NODE,
                                                                                                                "Start an unit server");
    action_parameter_interface->addParam(AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_LAUNCH_NODE_PAR_NAME,
                                         DataType::TYPE_STRING,
                                         "The name of the unit server to launch");
    action_parameter_interface->addParam(AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_LAUNCH_NODE_PAR_CFG,
                                         DataType::TYPE_STRING,
                                         "The content of init file of the unit servers");
    
    
    action_parameter_interface = DeclareAction::addActionDescritionInstance<ProcessWorker>(this,
                                                                                           &ProcessWorker::stopUnitServer,
                                                                                           getName(),
                                                                                           AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_STOP_NODE,
                                                                                           "Stop an unit server");
    action_parameter_interface->addParam(AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_STOP_NODE_PAR_NAME,
                                         DataType::TYPE_STRING,
                                         "The name of the unit server to launch");
    
    action_parameter_interface = DeclareAction::addActionDescritionInstance<ProcessWorker>(this,
                                                                                           &ProcessWorker::restartUnitServer,
                                                                                           getName(),
                                                                                           AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_RESTART_NODE,
                                                                                           "Restart an unit server");
    action_parameter_interface->addParam(AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_RESTART_NODE_PAR_NAME,
                                         DataType::TYPE_STRING,
                                         "The name of the unit server to launch");
    
    action_parameter_interface->addParam(AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_RESTART_NODE_PAR_KILL,
                                         DataType::TYPE_BOOLEAN,
                                         "Kill the process for stop it");
    
    action_parameter_interface = DeclareAction::addActionDescritionInstance<ProcessWorker>(this,
                                                                                           &ProcessWorker::listUnitServers,
                                                                                           getName(),
                                                                                           AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_LIST_NODE,
                                                                                           "List all unit server");
}

ProcessWorker::~ProcessWorker() {
    
}


void ProcessWorker::init(void *data) throw(chaos::CException) {
    
}

void ProcessWorker::deinit() throw(chaos::CException) {}


chaos::common::data::CDataWrapper *ProcessWorker::launchUnitServer(chaos::common::data::CDataWrapper *data,
                                                                    bool& detach) {
    CHECK_CDW_THROW_AND_LOG(data,
                            ERROR, -1,
                            CHAOS_FORMAT("[%1%] ACK message with no content", %getName()));
    CHECK_KEY_THROW_AND_LOG(data, AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_LAUNCH_NODE_PAR_NAME,
                            ERROR, -2, CHAOS_FORMAT("[%1%] No unit server name found", %getName()));
    CHECK_ASSERTION_THROW_AND_LOG((data->isStringValue(AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_LAUNCH_NODE_PAR_NAME)),
                                  ERROR, -3,
                                  CHAOS_FORMAT("[%1%] %2% key need to be a string", %getName()%AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_LAUNCH_NODE_PAR_NAME));
    if(data->hasKey(AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_LAUNCH_NODE_PAR_CFG)) {
        CHECK_ASSERTION_THROW_AND_LOG((data->isStringValue(AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_LAUNCH_NODE_PAR_CFG)),
                                      ERROR, -3,
                                      CHAOS_FORMAT("[%1%] %2% key need to be a string", %getName()%AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_LAUNCH_NODE_PAR_CFG));
    }
    AgentAssociationSDWrapper assoc_sd_wrapper;
    assoc_sd_wrapper.deserialize(data);
    launchProcess(assoc_sd_wrapper());
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

#pragma mark Process Utility

void ProcessWorker::launchProcess(const AgentAssociation& node_association_info) {
    std::string exec_command = CHAOS_FORMAT("%1%/%2%", %ChaosAgent::getInstance()->settings.working_directory%node_association_info.launch_cmd_line);
    boost::shared_ptr<FILE> pipe(popen(node_association_info.launch_cmd_line.c_str(), "r"), pclose);
    if (!pipe) throw chaos::CException(-1, "popen() failed!", __PRETTY_FUNCTION__);
}
