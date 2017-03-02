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
#include <chaos/common/utility/UUIDUtil.h>

#include <boost/filesystem.hpp>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
 #include <sys/types.h>
 #include <sys/wait.h>

#define INFO  INFO_LOG(ProcessWorker)
#define ERROR ERR_LOG(ProcessWorker)
#define DBG   DBG_LOG(ProcessWorker)

using namespace chaos::agent;
using namespace chaos::agent::worker;
using namespace chaos::service_common::data::agent;

using namespace chaos::common::data;
using namespace chaos::common::utility;

ProcessWorker::ProcessWorker():
AbstractWorker(AgentNodeDomainAndActionRPC::ProcessWorker::WORKER_NAME) {
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
        quitProcess(assoc_sd_wrapper());
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

using namespace std;
//got from http://stackoverflow.com/questions/26852198/getting-the-pid-from-popen
#define READ   0
#define WRITE  1
FILE * popen2(string command, string type, int & pid) {
    pid_t child_pid;
    int fd[2];
    pipe(fd);
    
    if((child_pid = fork()) == -1)     {
        perror("fork");
        exit(0);
    }
    
    /* child process */
    if (child_pid == 0) {
        if (type == "r") {
            close(fd[READ]);    //Close the READ end of the pipe since the child's fd is write-only
            dup2(fd[WRITE], 1); //Redirect stdout to pipe
        } else {
            close(fd[WRITE]);    //Close the WRITE end of the pipe since the child's fd is read-only
            dup2(fd[READ], 0);   //Redirect stdin to pipe
        }
        setpgid(0, 0); //Needed so negative PIDs can kill children of /bin/sh
        execl("/bin/sh", "/bin/sh", "-c", command.c_str(), NULL);
        exit(0);
    } else {
        if (type == "r") {
            close(fd[WRITE]); //Close the WRITE end of the pipe since parent's fd is read-only
        } else {
            close(fd[READ]); //Close the READ end of the pipe since parent's fd is write-only
        }
    }
    
    pid = child_pid;
    if (type == "r") {
        return fdopen(fd[READ], "r");
    }
    return fdopen(fd[WRITE], "w");
}

bool popen2NoPipe(string command, int & pid) {
    if((pid = fork()) == -1)     {
        return false;
    }
    
    /* child process */
    if (pid == 0) {
        setpgid(0, 0); //Needed so negative PIDs can kill children of /bin/sh
        execl("/bin/sh", "/bin/sh", "-c", command.c_str(), NULL);
        exit(0);
    }
    return true;
}

int pclose2(FILE * fp,
            pid_t pid,
            bool wait_pid = false) {
    int stat;
    
    fclose(fp);
    if(wait_pid) {
        while (waitpid(pid, &stat, 0) == -1) {
            if (errno != EINTR) {
                stat = -1;
                break;
            }
        }
    }
    return stat;
}

#define INIT_FILE_NAME(x)\
CHAOS_FORMAT("%1%.ini",%x.association_unique_id)

#define INIT_FILE_PATH()\
CHAOS_FORMAT("%1%/ini_files/", %ChaosAgent::getInstance()->settings.working_directory)

#define COMPOSE_NODE_LAUNCH_CMD_LINE(x)\
CHAOS_FORMAT("%1%/%2% --%3% %4%%5%", %ChaosAgent::getInstance()->settings.working_directory%x.launch_cmd_line%chaos::InitOption::OPT_CONF_FILE%INIT_FILE_PATH()%INIT_FILE_NAME(x))

void ProcessWorker::launchProcess(const AgentAssociation& node_association_info) {
    int pid = 0;
    boost::filesystem::path init_file;
    boost::filesystem::path exec_command;
    try{
        exec_command = COMPOSE_NODE_LAUNCH_CMD_LINE(node_association_info);
        init_file = CHAOS_FORMAT("%1%/%2%", %INIT_FILE_PATH()%INIT_FILE_NAME(node_association_info));
        boost::filesystem::path init_file_parent_path = INIT_FILE_PATH();
        
        if (boost::filesystem::exists(init_file_parent_path) == false &&
            boost::filesystem::create_directory(init_file_parent_path) == false) {
            throw chaos::CException(-1, CHAOS_FORMAT("Parent path %1% can't be created",%init_file_parent_path), __PRETTY_FUNCTION__);
        }
        
        //write configuration file
        std::ofstream init_file_stream;
        init_file_stream.open(init_file.string().c_str(), std::ofstream::trunc | std::ofstream::out);
        init_file_stream.write(node_association_info.configuration_file_content.c_str(), node_association_info.configuration_file_content.length());
        init_file_stream.close();
        if (!popen2NoPipe(exec_command.string().c_str(), pid)) {throw chaos::CException(-2, "popen() failed!", __PRETTY_FUNCTION__);}
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
    FILE *in = popen2(ps_command.c_str(), "r", pid);
    if (!in) {throw chaos::CException(-2, "popen() failed!", __PRETTY_FUNCTION__);}
    
    while(fgets(buff, sizeof(buff), in)!=NULL){
        INFO << buff;
        if(strstr(buff, "grep") == NULL) {
            found = strstr(buff, node_association_info.launch_cmd_line.c_str()) != NULL;
            if(found) break;
        }
    }
    pclose2(in, pid);
    return found;
}

bool ProcessWorker::quitProcess(const chaos::service_common::data::agent::AgentAssociation& node_association_info,
                                bool kill) {
    int pid = 0;
    const std::string exec_command = kill?CHAOS_FORMAT("pkill -SIGKILL -f \"%1%\"",%INIT_FILE_NAME(node_association_info)):CHAOS_FORMAT("pkill -SIGTERM -f \"%1%\"",%INIT_FILE_NAME(node_association_info));
    if (!popen2NoPipe(exec_command.c_str(), pid)) {throw chaos::CException(-2, "popen() failed!", __PRETTY_FUNCTION__);}
    return pid != 0;
}
