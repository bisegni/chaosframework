/*
 *	ProcUtil.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 06/03/2017 INFN, National Institute of Nuclear Physics
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

#include "ProcUtil.h"
#include "../ChaosAgent.h"
#include <chaos/common/global.h>
#include <chaos/common/configuration/GlobalConfiguration.h>

#define READ   0
#define WRITE  1

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;

using namespace chaos::common;

using namespace chaos::agent::utility;
using namespace chaos::service_common::data::agent;

FILE * ProcUtil::popen2(const std::string& command,
                        const std::string& type,
                        int & pid) {
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

bool ProcUtil::popen2NoPipe(const std::string& command,
                            int & pid) {
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

bool ProcUtil::popen2ToNamedPipe(const std::string& command,
                                 const std::string& named_pipe) {
    int pid, sid = 0;
    if((pid = fork()) == -1)     {
        return false;
    }
    
    if (pid == 0) {
        /* child process */
        if((pid = fork()) < 0){
            //error forking
            exit(EXIT_FAILURE);
        } else if(pid > 0) {
            //firs child exis because became parent
            exit(0);
        }
        
        //second child launch the process
        int fd = 0;
        
        /* Create a new SID for the child process */
        //        sid = setsid();
        //        if (sid < 0) {
        //            exit(EXIT_FAILURE);
        //        }
        setpgid(0, 0); //Needed so negative PIDs can kill children of /bin/sh
        //redirect standard output and error to the named pipe
        if((fd = open(named_pipe.c_str(), O_RDWR | O_CREAT,S_IRUSR | S_IWUSR))==-1){ /*open the file */
            perror("open");
            return 1;
        }
        
        dup2(fd,STDOUT_FILENO); /*copy the file descriptor fd into standard output*/
        dup2(fd,STDERR_FILENO); /* same, for the standard error */
        close(fd); /* close the file descriptor as we don't need it more  */
        
        execl("/bin/sh", "/bin/sh", "-c", command.c_str(), NULL);
    } else {
        int status;
        do {
            if(waitpid(pid, &status, 0) == -1) {
                if (errno != EINTR) {
                    break;
                }
            }
        } while (!WIFEXITED(status));
    }
    return true;
}

int ProcUtil::pclose2(FILE * fp,
                      pid_t pid) {
    int status;
    fclose(fp);
    do {
        if(waitpid(pid, &status, 0) == -1) {
            if (errno != EINTR) {
                status = -1;
                break;
            }
        }
    } while (!WIFEXITED(status));
    return status;
}

int ProcUtil::createNamedPipe(const std::string& named_pipe_path) {
    mkfifo(named_pipe_path.c_str(), 0666);
    return errno;
}

int ProcUtil::removeNamedPipe(const std::string& named_pipe_path) {
    unlink(named_pipe_path.c_str());
    return errno;
}

void ProcUtil::launchProcess(const AgentAssociation& node_association_info) {
    int pid = 0;
    std::string exec_command;
    boost::filesystem::path init_file;
    boost::filesystem::path queue_file;
    try{
        if(checkProcessAlive(node_association_info) == true) return;
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
        
        //enable log on console that will be redirected on named pipe
        init_file_stream << CHAOS_FORMAT("%1%=",%InitOption::OPT_LOG_ON_CONSOLE) << std::endl;
        
        //check for syslog setting of the agent that will be reflect on managed us
        if(GlobalConfiguration::getInstance()->hasOption(InitOption::OPT_LOG_ON_SYSLOG)) {
            init_file_stream << CHAOS_FORMAT("%1%=",%InitOption::OPT_LOG_ON_SYSLOG) << std::endl;
        }
        
        if(ChaosAgent::getInstance()->settings.enable_us_logging) {
            init_file_stream << CHAOS_FORMAT("%1%=",%InitOption::OPT_LOG_ON_FILE) << std::endl;
            init_file_stream << CHAOS_FORMAT("%1%=%2%/%3%",%InitOption::OPT_LOG_FILE%LOG_FILE_PATH()%LOG_FILE_NAME(node_association_info)) << std::endl;
        }

        
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
    } catch(std::exception& ex) {
        throw ex;
    } catch(chaos::CException& ex) {
        throw ex;
    }
}

bool ProcUtil::checkProcessAlive(const AgentAssociation& node_association_info) {
    int pid;
    bool found = false;
    char buff[512];
    std::string ps_command = CHAOS_FORMAT("ps -ef | grep '%1%'", %INIT_FILE_NAME(node_association_info));
    FILE *in = ProcUtil::popen2(ps_command.c_str(), "r", pid);
    if (!in) {throw chaos::CException(-2, "popen() failed!", __PRETTY_FUNCTION__);}
    
    while(fgets(buff, sizeof(buff), in)!=NULL){
        if(strstr(buff, "grep") == NULL) {
            found = strstr(buff, node_association_info.launch_cmd_line.c_str()) != NULL;
            if(found) break;
        }
    }
    ProcUtil::pclose2(in, pid);
    return found;
}

bool ProcUtil::quitProcess(const AgentAssociation& node_association_info,
                           bool kill) {
    int pid = 0;
    const std::string exec_command = kill?CHAOS_FORMAT("pkill -SIGKILL -f \"%1%\"",%INIT_FILE_NAME(node_association_info)):CHAOS_FORMAT("pkill -SIGTERM -f \"%1%\"",%INIT_FILE_NAME(node_association_info));
    if (!ProcUtil::popen2NoPipe(exec_command.c_str(), pid)) {throw chaos::CException(-2, "popen() failed!", __PRETTY_FUNCTION__);}
    return pid != 0;
}
