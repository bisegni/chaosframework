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
    code << "extern \"C\" int testFunction(const std::vector<chaos::common::data::CDataVariant>& i_var, std::vector<chaos::common::data::CDataVariant>& o_var) {"
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
    ASSERT_EQ(in_param[0].asInt32(), out_param[0].asInt32());

    ASSERT_EQ(out_param[1].getType(), chaos::DataType::TYPE_INT32);
    ASSERT_EQ(in_param[1].asInt64(), out_param[1].asInt64());

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
