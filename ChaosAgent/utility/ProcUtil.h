/*
 *	ProcUtil.h
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

#ifndef __CHAOSFramework_D11D47EC_B834_406C_897C_8191C80B6D8B_ProcUtil_h
#define __CHAOSFramework_D11D47EC_B834_406C_897C_8191C80B6D8B_ProcUtil_h

#include "../ChaosAgent.h"

#include <cstdio>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>

//got from http://stackoverflow.com/questions/26852198/getting-the-pid-from-popen

namespace chaos {
    namespace agent {
        namespace utility {
            
            
#define INIT_FILE_NAME(x)\
CHAOS_FORMAT("%1%_%2%.ini",%x.association_unique_id%x.associated_node_uid)
            
#define LOG_FILE_NAME(x)\
CHAOS_FORMAT("%1%_%2%_%%Y-%%m-%%d_%%H-%%M-%%S.%%N.log",%x.association_unique_id%x.associated_node_uid)
            
#define NPIPE_FILE_NAME(x)\
CHAOS_FORMAT("%1%.pipe",%x.association_unique_id)
            
#define INIT_FILE_PATH()\
CHAOS_FORMAT("%1%/ini_files/", %ChaosAgent::getInstance()->settings.working_directory)
            
#define LOG_FILE_PATH()\
CHAOS_FORMAT("%1%/log/", %ChaosAgent::getInstance()->settings.working_directory)
            
#define QUEUE_FILE_PATH()\
CHAOS_FORMAT("%1%/queue/", %ChaosAgent::getInstance()->settings.working_directory)
            
#define COMPOSE_NODE_LAUNCH_CMD_LINE(x)\
CHAOS_FORMAT("%1%/bin/%2% --%3% %4%%5%", %ChaosAgent::getInstance()->settings.working_directory%x.launch_cmd_line%chaos::InitOption::OPT_CONF_FILE%INIT_FILE_PATH()%INIT_FILE_NAME(x))
            
            class ProcUtil {
            public:
                static FILE * popen2(const std::string& command,
                                     const std::string& type,
                                     int & pid);
                
                static bool popen2NoPipe(const std::string& command,
                                         int & pid);
                
                static bool popen2ToNamedPipe(const std::string& command,
                                              const std::string& named_pipe);
                
                static int pclose2(FILE * fp,
                                   pid_t pid);
                
                static int createNamedPipe(const std::string& named_pipe_path);
                
                static int removeNamedPipe(const std::string& named_pipe_path);
                
                static void launchProcess(const chaos::service_common::data::agent::AgentAssociation& node_association_info);
                
                static bool checkProcessAlive(const chaos::service_common::data::agent::AgentAssociation& node_association_info);
                
                static bool quitProcess(const chaos::service_common::data::agent::AgentAssociation& node_association_info, bool kill = false);
            };
        }
    }
}

#endif /* __CHAOSFramework_D11D47EC_B834_406C_897C_8191C80B6D8B_ProcUtil_h */
