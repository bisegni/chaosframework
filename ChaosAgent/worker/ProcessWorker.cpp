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

#include "ProcessWorker.h"
#include "../utility/ProcUtil.h"
#include <ChaosAgent/utility/ProcUtil.h>

#include <chaos_service_common/data/node/Agent.h>

#include <chaos/common/chaos_constants.h>
#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/configuration/GlobalConfiguration.h>

#include <boost/filesystem.hpp>


#define INFO  INFO_LOG(ProcessWorker)
#define ERROR ERR_LOG(ProcessWorker)
#define DBG   DBG_LOG(ProcessWorker)

using namespace chaos::agent;
using namespace chaos::agent::worker;
using namespace chaos::agent::utility;
using namespace chaos::service_common::data::agent;

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::async_central;

ProcessWorker::ProcessWorker():
AbstractWorker(AgentNodeDomainAndActionRPC::ProcessWorker::RPC_DOMAIN) {
    //register rpc action
    AbstActionDescShrPtr action_parameter_interface = DeclareAction::addActionDescritionInstance<ProcessWorker>(this,
                                                                                                                &ProcessWorker::launchNode,
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
                                                                                           &ProcessWorker::stopNode,
                                                                                           getName(),
                                                                                           AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_STOP_NODE,
                                                                                           "Stop an unit server");
    action_parameter_interface->addParam(AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_STOP_NODE_PAR_NAME,
                                         DataType::TYPE_STRING,
                                         "The name of the unit server to launch");
    
    action_parameter_interface = DeclareAction::addActionDescritionInstance<ProcessWorker>(this,
                                                                                           &ProcessWorker::restartNode,
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
                                                                                           &ProcessWorker::checkNodes,
                                                                                           getName(),
                                                                                           AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_CHECK_NODE,
                                                                                           "Check node status");
    action_parameter_interface->addParam(AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_CHECK_NODE_ASSOCIATED_NODES,
                                         DataType::TYPE_UNDEFINED,
                                         "Need to be a vector of node association");
}

ProcessWorker::~ProcessWorker() {}

void ProcessWorker::init(void *data) throw(chaos::CException) {
    AsyncCentralManager::getInstance()->addTimer(this, 0, chaos::common::constants::AgentTimersTimeoutinMSec);
}

void ProcessWorker::deinit() throw(chaos::CException) {
    AsyncCentralManager::getInstance()->removeTimer(this);
}


void ProcessWorker::timeout() {
    LockableMapRespawnableNodeReadLock rl = map_respawnable_node.getReadLockObject();
    for(MapRespawnableNodeIterator it = map_respawnable_node().begin(),
        end = map_respawnable_node().end();
        it != end;
        it++) {
        if(ProcUtil::checkProcessAlive(it->second) == false) {
            INFO << CHAOS_FORMAT("Respawn process for node %1%", %it->first);
            ProcUtil::launchProcess(it->second);
        }
    }
}

void ProcessWorker::addToRespawn(const chaos::service_common::data::agent::AgentAssociation& node_association_info) {
    LockableMapRespawnableNodeWriteLock wr = map_respawnable_node.getWriteLockObject();
    map_respawnable_node()[node_association_info.associated_node_uid] = node_association_info;
    INFO << CHAOS_FORMAT("Node %1% added to auto respawn check", %node_association_info.associated_node_uid);
}

void ProcessWorker::removeToRespawn(const std::string& node_uid) {
    LockableMapRespawnableNodeWriteLock wr = map_respawnable_node.getWriteLockObject();
    map_respawnable_node().erase(node_uid);
    INFO << CHAOS_FORMAT("Node %1% removed from auto respawn check", %node_uid);
}

CDWUniquePtr ProcessWorker::launchNode(CDWUniquePtr data) {
    CHECK_ASSERTION_THROW_AND_LOG(data.get()!=NULL,
                                  ERROR, -1,
                                  CHAOS_FORMAT("[%1%] ACK message with no content", %getName()));
    CHECK_KEY_THROW_AND_LOG(data,
                            AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_LAUNCH_NODE_PAR_NAME,
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
    assoc_sd_wrapper.deserialize(data.get());
    if(ProcUtil::checkProcessAlive(assoc_sd_wrapper()) == false) {
        ProcUtil::launchProcess(assoc_sd_wrapper());
    }
    if(assoc_sd_wrapper().keep_alive) {
        addToRespawn(assoc_sd_wrapper());
    }
    return CDWUniquePtr();
}

CDWUniquePtr ProcessWorker::stopNode(CDWUniquePtr data) {
    AgentAssociationSDWrapper assoc_sd_wrapper;
    assoc_sd_wrapper.deserialize(data.get());
    if(ProcUtil::checkProcessAlive(assoc_sd_wrapper()) == true) {
        bool kill = false;
        if(data.get()!=NULL && data->hasKey(AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_RESTART_NODE_PAR_KILL)) {
            kill = data->getBoolValue(AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_RESTART_NODE_PAR_KILL);
        }
        ProcUtil::quitProcess(assoc_sd_wrapper(), kill);
    } else {
        INFO << CHAOS_FORMAT("Associated node %1% isn't in execution", %assoc_sd_wrapper().associated_node_uid);
    }
    removeToRespawn(assoc_sd_wrapper().associated_node_uid);
    return CDWUniquePtr();
}

CDWUniquePtr ProcessWorker::restartNode(CDWUniquePtr data) {
    bool try_to_stop = true;
    int retry = 0;
    bool process_alive = false;
    
    AgentAssociationSDWrapper assoc_sd_wrapper;
    assoc_sd_wrapper.deserialize(data.get());
    
    //first remove node to respawn
    removeToRespawn(assoc_sd_wrapper().associated_node_uid);
    if(ProcUtil::checkProcessAlive(assoc_sd_wrapper()) == true) {
        ProcUtil::quitProcess(assoc_sd_wrapper());
        while((process_alive = ProcUtil::checkProcessAlive(assoc_sd_wrapper())) == false &&
              try_to_stop) {
            if(retry++ > 3) {
                try_to_stop = false;
                //try to kill process
                ProcUtil::quitProcess(assoc_sd_wrapper(), true);
                //exit without check
            } else {
                sleep(1);
            }
        }
    }
    if((process_alive = process_alive || ProcUtil::checkProcessAlive(assoc_sd_wrapper())) == false) {
        //process has been shutdown, restart it
        ProcUtil::launchProcess(assoc_sd_wrapper());
        
        if(assoc_sd_wrapper().keep_alive) {
            addToRespawn(assoc_sd_wrapper());
        }
    }
    return CDWUniquePtr();
}

CDWUniquePtr ProcessWorker::checkNodes(CDWUniquePtr data) {
    VectorAgentAssociationStatusSDWrapper result_status_vec_sd_wrapper;
    result_status_vec_sd_wrapper.serialization_key = AgentNodeDefinitionKey::NODE_ASSOCIATED;
    
    VectorAgentAssociationSDWrapper associated_node_sd_wrapper;
    associated_node_sd_wrapper.serialization_key = AgentNodeDefinitionKey::NODE_ASSOCIATED;
    
    associated_node_sd_wrapper.deserialize(data.get());
    if(associated_node_sd_wrapper().size()) {
        for(VectorAgentAssociationIterator it = associated_node_sd_wrapper().begin(),
            end = associated_node_sd_wrapper().end();
            it != end;
            it++) {
            AgentAssociationStatus status;
            status.associated_node_uid = it->associated_node_uid;
            status.alive = ProcUtil::checkProcessAlive(*it);
            result_status_vec_sd_wrapper().push_back(status);
        }
    }
    return result_status_vec_sd_wrapper.serialize();
}
