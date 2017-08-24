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

#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>


#include <chaos/common/script/ScriptManager.h>

using namespace chaos::common::data;
using namespace chaos::common::script;
using namespace chaos::common::utility;

ChaosUniquePtr<ScriptManager> script_manager;

int main(int argc, const char * argv[]) {
    script_manager.reset(new ScriptManager("Lua"));
    InizializableService::initImplementation(script_manager.get(),
                                             NULL,
                                             "ScriptManager",
                                             __PRETTY_FUNCTION__);
    
    
    std::ifstream t("test.lua");
    std::string script((std::istreambuf_iterator<char>(t)),
                       std::istreambuf_iterator<char>());
    script_manager->getVirtualMachine()->loadScript(script);
    
    ScriptInParam in_param;
    in_param.push_back(CDataVariant("test_string"));
    script_manager->getVirtualMachine()->callProcedure("testString", in_param);
    
    in_param.clear();
    in_param.push_back(CDataVariant("wrong_string"));
    script_manager->getVirtualMachine()->callProcedure("testString", in_param);
    
    InizializableService::deinitImplementation(script_manager.get(),
                                               "ScriptManager",
                                               __PRETTY_FUNCTION__);
    return 0;
}
