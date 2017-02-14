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

#define INFO  INFO_LOG(ProcessWorker)
#define ERROR ERR_LOG(ProcessWorker)
#define DBG   DBG_LOG(ProcessWorker)

using namespace chaos::agent;
using namespace chaos::agent::worker;
using namespace chaos::service_common::data::agent;

using namespace chaos::common::utility;

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
    assoc_sd_wrapper().launch_cmd_line = launchProcess(assoc_sd_wrapper());
    if(checkProcessAlive(assoc_sd_wrapper())) {
        INFO << "Alive";
    } else {
        INFO << "Dead";
    }
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

std::string ProcessWorker::launchProcess(const AgentAssociation& node_association_info) {
    int pid = 0;
    bool use_init_file = false;
    std::string init_file_name;
    boost::filesystem::path exec_command;
    try{
        exec_command = CHAOS_FORMAT("%1%/%2%", %ChaosAgent::getInstance()->settings.working_directory%node_association_info.launch_cmd_line);
        boost::filesystem::path init_file = CHAOS_FORMAT("%1%/ini_files/%2%", %ChaosAgent::getInstance()->settings.working_directory%(init_file_name = UUIDUtil::generateUUIDLite()+".ini"));
        boost::filesystem::path init_file_parent_path = init_file.parent_path();
        
        if (boost::filesystem::exists(init_file_parent_path) == false &&
            boost::filesystem::create_directory(init_file_parent_path) == false) {
            throw chaos::CException(-1, CHAOS_FORMAT("Parent path %1% can't be created",%init_file_parent_path), __PRETTY_FUNCTION__);
        }
        //write config file
        if((use_init_file = node_association_info.configuration_file_content.size() > 0)) {
            //we have also a configuraiton file
            
            //write configuration file
            std::ofstream init_file_stream;
            init_file_stream.open(init_file.string().c_str(), std::ofstream::trunc | std::ofstream::out);
            init_file_stream.write(node_association_info.configuration_file_content.c_str(), node_association_info.configuration_file_content.length());
            init_file_stream.close();
            
            //add parameter to launch comamnd line for read the configuraiton file
            exec_command += CHAOS_FORMAT(" --conf-file=%1%", %init_file_name);
        }
        
        FILE *pipe = popen2(exec_command.string().c_str(), "r", pid);
        if (!pipe) {throw chaos::CException(-2, "popen() failed!", __PRETTY_FUNCTION__);}
        pclose2(pipe, pid);
        if(use_init_file){boost::filesystem::remove(init_file);}
    } catch(std::exception& e) {
        ERROR << e.what();
    }
    return exec_command.string();
}

bool ProcessWorker::checkProcessAlive(const chaos::service_common::data::agent::AgentAssociation& node_association_info) {
    int pid;
    bool found = false;
    char buff[512];
    std::string ps_command = CHAOS_FORMAT("ps -ef | grep '%1%'", %node_association_info.launch_cmd_line);
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
