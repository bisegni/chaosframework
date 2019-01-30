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

#include <ChaosAgent/ChaosAgent.h>
#include <ChaosAgent/chaos_agent_constants.h>

using namespace chaos::agent;

int main(int argc, const char * argv[]) {
    try {
        ChaosAgent::getInstance()->getGlobalConfigurationInstance()->addOption< int >(OPT_REST_PORT,
                                                                                              "Port to query process management",
                                                                                              8071,
                                                                                              &ChaosAgent::getInstance()->settings.restport);
        //data worker
        ChaosAgent::getInstance()->getGlobalConfigurationInstance()->addOption< std::string >(OPT_WORKING_DIR,
                                                                                              "Working directory for agent",
                                                                                              "",
                                                                                              &ChaosAgent::getInstance()->settings.working_directory);
        ChaosAgent::getInstance()->getGlobalConfigurationInstance()->addOption< bool >(OPT_ENABLE_EXT_CMD,
                                                                                              "Enable the external command via pipe",
                                                                                              false,
                                                                                              &ChaosAgent::getInstance()->settings.ext_cmd_enabled);
        ChaosAgent::getInstance()->getGlobalConfigurationInstance()->addOption< bool >(OPT_ENABLE_SEPARATE_US_LOG,
                                                                                       "Enable separate file logging of managed us",
                                                                                       false,
                                                                                       &ChaosAgent::getInstance()->settings.enable_us_logging);
        ChaosAgent::getInstance()->getGlobalConfigurationInstance()->addOption< bool >(OPT_ENABLE_MERGE_US_LOG,
                                                                                       "Merge the us managed log into the agent log",
                                                                                       false,
                                                                                       &ChaosAgent::getInstance()->settings.enable_us_merge_logging);
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
