/*
 *	ChaosAgent.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 03/02/2017 INFN, National Institute of Nuclear Physics
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

#include <ChaosAgent/ChaosAgent.h>
#include <ChaosAgent/chaos_agent_constants.h>

using namespace chaos::agent;

int main(int argc, char * argv[]) {
    try {
        //data worker
        ChaosAgent::getInstance()->getGlobalConfigurationInstance()->addOption< std::string >(OPT_WORKING_DIR,
                                                                                              "Working directory for agent",
                                                                                              "",
                                                                                              &ChaosAgent::getInstance()->settings.working_directory);
        ChaosAgent::getInstance()->getGlobalConfigurationInstance()->addOption< bool >(OPT_ENABLE_EXT_CMD,
                                                                                              "Enable the external command via pipe",
                                                                                              false,
                                                                                              &ChaosAgent::getInstance()->settings.ext_cmd_enabled);
        ChaosAgent::getInstance()->getGlobalConfigurationInstance()->addOption< std::string >(OPT_EXT_CMD_PIPE_PATH,
                                                                                              "Path for the external command pipe",
                                                                                              "/tmp",
                                                                                              &ChaosAgent::getInstance()->settings.ext_cmd_pipe_path);
        ChaosAgent::getInstance()->getGlobalConfigurationInstance()->addOption< std::string >(OPT_EXT_CMD_PIPE_NAME,
                                                                                              "The name of the pipe (name.in name.out) for listen for external command",
                                                                                              "chaos_agent_ext_cmd",
                                                                                              &ChaosAgent::getInstance()->settings.ext_cmd_pipe_name);
        ChaosAgent::getInstance()->init(argc, argv);
        
        ChaosAgent::getInstance()->start();
    } catch (chaos::CException& ex) {
        DECODE_CHAOS_EXCEPTION(ex)
    }
    CHAOS_NOT_THROW(ChaosAgent::getInstance()->stop(););
    CHAOS_NOT_THROW(ChaosAgent::getInstance()->deinit(););
    return 0;
}
