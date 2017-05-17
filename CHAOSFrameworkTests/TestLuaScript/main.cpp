//
//  main.cpp
//  TestLuaScript
//
//  Created by Claudio Bisegni on 07/04/2017.
//  Copyright © 2017 bisegni. All rights reserved.
//

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
