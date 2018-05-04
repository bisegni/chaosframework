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

#include "ScriptClingTest.h"

using namespace chaos::common::data;
using namespace chaos::common::script;
using namespace chaos::common::utility;


#pragma mark TestApi
TestApi::TestApi():
TemplatedAbstractScriptableClass(this,
                                 "TestApiClassName"){
    //add all exposed api
    addApi("echo", &TestApi::echo);
}

TestApi::~TestApi() {}


int TestApi::echo(const ScriptInParam& input_parameter,
                  ScriptOutParam& output_parameter) {
    for(ScriptInParamConstIterator it = input_parameter.begin(),
        end = input_parameter.end();
        it != end;
        it++) {
        output_parameter.push_back(*it);
    }
    return 0;
}

#pragma mark Tests
TEST(ScriptClingTest, InitDeinit) {
    ChaosUniquePtr<ScriptManager> script_manager(new ScriptManager("CPP"));
    ASSERT_NO_THROW(InizializableService::initImplementation(script_manager.get(),
                                                             NULL,
                                                             "ScriptManager",
                                                             __PRETTY_FUNCTION__););
    
    InizializableService::deinitImplementation(script_manager.get(),
                                               "ScriptManager",
                                               __PRETTY_FUNCTION__);
}

TEST(ScriptClingTest, ExecuteFunction) {
    ScriptInParam in_param;
    ScriptInParam out_param;
    std::ostringstream code;
    ChaosUniquePtr<ScriptManager> script_manager(new ScriptManager("CPP"));
    ASSERT_NO_THROW(InizializableService::initImplementation(script_manager.get(),
                                                             NULL,
                                                             "ScriptManager",
                                                             __PRETTY_FUNCTION__););
    bool exists = false;
    code << "#include <queue>\n"
    "extern \"C\" int testFunction(const std::vector<chaos::common::data::CDataVariant>& i_var, std::vector<chaos::common::data::CDataVariant>& o_var) {"
    "for(const chaos::common::data::CDataVariant& i : i_var) {o_var.push_back(i);}"
    "return 0;}";
    ASSERT_EQ(script_manager->getVirtualMachine()->loadScript(code.str()), 0);
    ASSERT_EQ(script_manager->getVirtualMachine()->functionExists("testFunction", exists), 0);
    ASSERT_TRUE(exists);
    
    in_param.push_back(CDataVariant(false));
    in_param.push_back(CDataVariant((int32_t)32));
    in_param.push_back(CDataVariant((int64_t)64));
    in_param.push_back(CDataVariant(12.5));
    in_param.push_back(CDataVariant("String"));
    ASSERT_EQ(script_manager->getVirtualMachine()->callFunction("testFunction", in_param, out_param), 0);
    
    //check result
    ASSERT_EQ(in_param.size(), in_param.size());
    
    ASSERT_EQ(out_param[0].getType(), chaos::DataType::TYPE_BOOLEAN);
    ASSERT_EQ(in_param[0].asBool(), out_param[0].asBool());
    
    ASSERT_EQ(out_param[1].getType(), chaos::DataType::TYPE_INT32);
    ASSERT_EQ(in_param[1].asInt32(), out_param[1].asInt32());
    
    ASSERT_EQ(out_param[2].getType(), chaos::DataType::TYPE_INT64);
    ASSERT_EQ(in_param[2].asInt64(), out_param[2].asInt64());
    
    ASSERT_EQ(out_param[3].getType(), chaos::DataType::TYPE_DOUBLE);
    ASSERT_DOUBLE_EQ(in_param[3].asDouble(), out_param[3].asDouble());
    
    ASSERT_EQ(out_param[4].getType(), chaos::DataType::TYPE_STRING);
    ASSERT_STREQ(in_param[4].asString().c_str(), out_param[4].asString().c_str());
    
    InizializableService::deinitImplementation(script_manager.get(),
                                               "ScriptManager",
                                               __PRETTY_FUNCTION__);
}

TEST(ScriptClingTest, ExecuteApi) {
    TestApi test_api;
    ScriptInParam in_param;
    ScriptInParam out_param;
    std::ostringstream code;
    ChaosUniquePtr<ScriptManager> script_manager(new ScriptManager("CPP"));
    ASSERT_NO_THROW(InizializableService::initImplementation(script_manager.get(),
                                                             NULL,
                                                             "ScriptManager",
                                                             __PRETTY_FUNCTION__););
    bool exists = false;
    code << "extern \"C\" int testApi(const std::vector<chaos::common::data::CDataVariant>& i_var, std::vector<chaos::common::data::CDataVariant>& o_var) {\n"
    "if(i_var.size() < 3) return -1;\n"
//    "std::cout << \"two\" << std::endl;\n"
    "const std::string& api_class  = i_var[0].asString();"
//    "std::cout << api_class << std::endl;\n"
    "const std::string& api_name  = i_var[1].asString();"
//    "std::cout << api_name << std::endl;\n"
    "return chaos_api.callScriptApi(api_class, api_name, i_var, o_var);\n"
    "}";
    ASSERT_NO_THROW(script_manager->registerApiClass(test_api));
//    ASSERT_EQ(script_manager->getVirtualMachine()->loadScript("#include<iostream>"), 0);
    ASSERT_EQ(script_manager->getVirtualMachine()->loadScript(code.str()), 0);
    ASSERT_EQ(script_manager->getVirtualMachine()->functionExists("testApi", exists), 0);
    ASSERT_TRUE(exists);
    
    
    in_param.push_back(CDataVariant("TestApiClassName"));
    in_param.push_back(CDataVariant("echo"));
    in_param.push_back(CDataVariant(false));
    in_param.push_back(CDataVariant((int32_t)32));
    in_param.push_back(CDataVariant((int64_t)64));
    in_param.push_back(CDataVariant(12.5));
    in_param.push_back(CDataVariant("String"));
    ASSERT_EQ(script_manager->getVirtualMachine()->callFunction("testApi", in_param, out_param), 0);
    //check result
    ASSERT_EQ(in_param.size(), in_param.size());
    
    ASSERT_EQ(out_param[0].getType(), chaos::DataType::TYPE_STRING);
    ASSERT_STREQ(in_param[0].asString().c_str(), out_param[0].asString().c_str());
    
    ASSERT_EQ(out_param[1].getType(), chaos::DataType::TYPE_STRING);
    ASSERT_STREQ(in_param[1].asString().c_str(), out_param[1].asString().c_str());
    
    ASSERT_EQ(out_param[2].getType(), chaos::DataType::TYPE_BOOLEAN);
    ASSERT_EQ(in_param[2].asBool(), out_param[2].asBool());
    
    ASSERT_EQ(out_param[3].getType(), chaos::DataType::TYPE_INT32);
    ASSERT_EQ(in_param[3].asInt32(), out_param[3].asInt32());
    
    ASSERT_EQ(out_param[4].getType(), chaos::DataType::TYPE_INT64);
    ASSERT_EQ(in_param[4].asInt64(), out_param[4].asInt64());
    
    ASSERT_EQ(out_param[5].getType(), chaos::DataType::TYPE_DOUBLE);
    ASSERT_DOUBLE_EQ(in_param[5].asDouble(), out_param[5].asDouble());
    
    ASSERT_EQ(out_param[6].getType(), chaos::DataType::TYPE_STRING);
    ASSERT_STREQ(in_param[6].asString().c_str(), out_param[6].asString().c_str());
    
    
    
    InizializableService::deinitImplementation(script_manager.get(),
                                               "ScriptManager",
                                               __PRETTY_FUNCTION__);
}
