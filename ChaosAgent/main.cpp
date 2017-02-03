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
                                                                                              "Working directory for deploy and porcess management",
                                                                                              "",
                                                                                              &ChaosAgent::getInstance()->settings.working_directory);
        
        ChaosAgent::getInstance()->init(argc, argv);
        
        ChaosAgent::getInstance()->start();
    } catch (chaos::CException& ex) {
        DECODE_CHAOS_EXCEPTION(ex)
    }
    CHAOS_NOT_THROW(ChaosAgent::getInstance()->stop(););
    CHAOS_NOT_THROW(ChaosAgent::getInstance()->deinit(););
    return 0;
}
