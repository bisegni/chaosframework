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
#include "ProcUtil.h"

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
using namespace chaos::service_common::data::agent;

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::utility;


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

ProcessWorker::~ProcessWorker() {
    
}


void ProcessWorker::init(void *data) throw(chaos::CException) {
    
}

void ProcessWorker::deinit() throw(chaos::CException) {}


chaos::common::data::CDataWrapper *ProcessWorker::launchNode(chaos::common::data::CDataWrapper *data,
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
    if(checkProcessAlive(assoc_sd_wrapper()) == false) {
        launchProcess(assoc_sd_wrapper());
    }
    return NULL;
}

chaos::common::data::CDataWrapper *ProcessWorker::stopNode(chaos::common::data::CDataWrapper *data,
                                                           bool& detach) {
    AgentAssociationSDWrapper assoc_sd_wrapper;
    assoc_sd_wrapper.deserialize(data);
    if(checkProcessAlive(assoc_sd_wrapper()) == true) {
        bool kill = false;
        if(data!=NULL && data->hasKey(AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_RESTART_NODE_PAR_KILL)) {
            kill = data->getBoolValue(AgentNodeDomainAndActionRPC::ProcessWorker::ACTION_RESTART_NODE_PAR_KILL);
        }
        quitProcess(assoc_sd_wrapper(), kill);
    } else {
        INFO << CHAOS_FORMAT("Associated node %1% isn't in execution", %assoc_sd_wrapper().associated_node_uid);
    }
    return NULL;
}

chaos::common::data::CDataWrapper *ProcessWorker::restartNode(chaos::common::data::CDataWrapper *data,
                                                              bool& detach) {
    bool try_to_stop = true;
    int retry = 0;
    bool process_alive = false;
    AgentAssociationSDWrapper assoc_sd_wrapper;
    assoc_sd_wrapper.deserialize(data);
    if(checkProcessAlive(assoc_sd_wrapper()) == true) {
        quitProcess(assoc_sd_wrapper());
        while((process_alive = checkProcessAlive(assoc_sd_wrapper())) == false &&
              try_to_stop) {
            if(retry++ > 3) {
                try_to_stop = false;
                //try to kill process
                quitProcess(assoc_sd_wrapper(), true);
                //exit without check
            } else {
                sleep(1);
            }
        }
    }
    if((process_alive = process_alive || checkProcessAlive(assoc_sd_wrapper())) == false) {
        //process has been shutdown, restart it
        launchProcess(assoc_sd_wrapper());
    }
    
    return NULL;
}

chaos::common::data::CDataWrapper *ProcessWorker::checkNodes(chaos::common::data::CDataWrapper *data,
                                                             bool& detach) {
    VectorAgentAssociationStatusSDWrapper result_status_vec_sd_wrapper;
    result_status_vec_sd_wrapper.serialization_key = AgentNodeDefinitionKey::NODE_ASSOCIATED;
    
    VectorAgentAssociationSDWrapper associated_node_sd_wrapper;
    associated_node_sd_wrapper.serialization_key = AgentNodeDefinitionKey::NODE_ASSOCIATED;
    
    associated_node_sd_wrapper.deserialize(data);
    if(associated_node_sd_wrapper().size()) {
        for(VectorAgentAssociationIterator it = associated_node_sd_wrapper().begin(),
            end = associated_node_sd_wrapper().end();
            it != end;
            it++) {
            AgentAssociationStatus status;
            status.associated_node_uid = it->associated_node_uid;
            status.alive = checkProcessAlive(*it);
            result_status_vec_sd_wrapper().push_back(status);
        }
    }
    return result_status_vec_sd_wrapper.serialize().release();
}

#pragma mark Process Utility

void ProcessWorker::launchProcess(const AgentAssociation& node_association_info) {
    int pid = 0;
    std::string exec_command;
    boost::filesystem::path init_file;
    boost::filesystem::path queue_file;
    try{
        exec_command = COMPOSE_NODE_LAUNCH_CMD_LINE(node_association_info);
        init_file = CHAOS_FORMAT("%1%/%2%", %INIT_FILE_PATH()%INIT_FILE_NAME(node_association_info));
        queue_file = CHAOS_FORMAT("%1%/%2%", %QUEUE_FILE_PATH()%NPIPE_FILE_NAME(node_association_info));
        
        boost::filesystem::path init_file_parent_path = INIT_FILE_PATH();
        if (boost::filesystem::exists(init_file_parent_path) == false &&
            boost::filesystem::create_directory(init_file_parent_path) == false) {
            throw chaos::CException(-1, CHAOS_FORMAT("Parent path %1% can't be created",%init_file_parent_path), __PRETTY_FUNCTION__);
        }
        
        boost::filesystem::path queue_file_parent_path = QUEUE_FILE_PATH();
        if (boost::filesystem::exists(queue_file_parent_path) == false &&
            boost::filesystem::create_directory(queue_file_parent_path) == false) {
            throw chaos::CException(-1, CHAOS_FORMAT("Queue path %1% can't be created",%queue_file_parent_path), __PRETTY_FUNCTION__);
        }
        
        //write configuration file
        std::ofstream init_file_stream;
        init_file_stream.open(init_file.string().c_str(), std::ofstream::trunc | std::ofstream::out);
        
        //append unit server alias
        init_file_stream << CHAOS_FORMAT("%1%=",%InitOption::OPT_LOG_ON_CONSOLE) << std::endl;
        init_file_stream << CHAOS_FORMAT("unit-server-alias=%1%",%node_association_info.associated_node_uid) << std::endl;
        
        //append metadata server from agent configuration
        VectorMetadatserver mds_vec = GlobalConfiguration::getInstance()->getMetadataServerAddressList();
        for(VectorMetadatserverIterator mds_it = mds_vec.begin(),
            end = mds_vec.end();
            mds_it != end;
            mds_it++) {
            init_file_stream << CHAOS_FORMAT("metadata-server=%1%",%mds_it->ip_port) << std::endl;
        }
        
        //append user defined paramenter
        init_file_stream.write(node_association_info.configuration_file_content.c_str(), node_association_info.configuration_file_content.length());
        init_file_stream.close();
        //create the named pipe
        ProcUtil::createNamedPipe(queue_file.string());
        if (!ProcUtil::popen2ToNamedPipe(exec_command.c_str(), queue_file.string())) {throw chaos::CException(-2, "popen() failed!", __PRETTY_FUNCTION__);}
    } catch(std::exception& e) {
        ERROR << e.what();
    } catch(chaos::CException& ex) {
        throw ex;
    }
}

bool ProcessWorker::checkProcessAlive(const chaos::service_common::data::agent::AgentAssociation& node_association_info) {
    int pid;
    bool found = false;
    char buff[512];
    std::string ps_command = CHAOS_FORMAT("ps -ef | grep '%1%'", %INIT_FILE_NAME(node_association_info));
    FILE *in = ProcUtil::popen2(ps_command.c_str(), "r", pid);
    if (!in) {throw chaos::CException(-2, "popen() failed!", __PRETTY_FUNCTION__);}
    
    while(fgets(buff, sizeof(buff), in)!=NULL){
        INFO << buff;
        if(strstr(buff, "grep") == NULL) {
            found = strstr(buff, node_association_info.launch_cmd_line.c_str()) != NULL;
            if(found) break;
        }
    }
    ProcUtil::pclose2(in, pid);
    return found;
}

bool ProcessWorker::quitProcess(const chaos::service_common::data::agent::AgentAssociation& node_association_info,
                                bool kill) {
    int pid = 0;
    const std::string exec_command = kill?CHAOS_FORMAT("pkill -SIGKILL -f \"%1%\"",%INIT_FILE_NAME(node_association_info)):CHAOS_FORMAT("pkill -SIGTERM -f \"%1%\"",%INIT_FILE_NAME(node_association_info));
    if (!ProcUtil::popen2NoPipe(exec_command.c_str(), pid)) {throw chaos::CException(-2, "popen() failed!", __PRETTY_FUNCTION__);}
    return pid != 0;
}
