/*
 * Copyright 2012, 16/10/2017 INFN
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
#include <gtest/gtest.h>

#include <chaos/common/script/ScriptManager.h>

using namespace chaos::common::data;
using namespace chaos::common::script;
using namespace chaos::common::utility;


TEST(TestScript, LuaScirpt) {
    ScriptInParam in_param;
    ScriptInParam out_param;
    std::string script("function testString(str_to_test) if(string.match(str_to_test, \"test_string\")) then print(\" String \"..str_to_test..\" has been matched\") return true else print(\" String\"..str_to_test..\" didn't match\") return false end end");
    ChaosUniquePtr<ScriptManager> script_manager(new ScriptManager("Lua"));
    ASSERT_NO_THROW(InizializableService::initImplementation(script_manager.get(),
                                                             NULL,
                                                             "ScriptManager",
                                                             __PRETTY_FUNCTION__););
    ASSERT_EQ(script_manager->getVirtualMachine()->loadScript(script), 0);
    
    in_param.push_back(CDataVariant("test_string"));
    ASSERT_EQ(script_manager->getVirtualMachine()->callFunction("testString", in_param, out_param), 0);
    ASSERT_EQ(out_param.size(), 1);
    ASSERT_TRUE(out_param[0].asBool());
    
    in_param.clear();
    out_param.clear();
    in_param.push_back(CDataVariant("wrong_string"));
    ASSERT_EQ(script_manager->getVirtualMachine()->callFunction("testString", in_param, out_param), 0);
    ASSERT_EQ(out_param.size(), 1);
    ASSERT_FALSE(out_param[0].asBool());
    
    InizializableService::deinitImplementation(script_manager.get(),
                                               "ScriptManager",
                                               __PRETTY_FUNCTION__);
}
