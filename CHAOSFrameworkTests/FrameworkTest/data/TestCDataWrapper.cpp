/*
 * Copyright 2012, 26/10/2017 INFN
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
#include <chaos/common/chaos_types.h>
#include <chaos/common/data/CDataWrapper.h>
#include <gtest/gtest.h>
#include <boost/lexical_cast.hpp>
using namespace chaos::common::data;

TEST(CDataWrapperTest, Normal) {
    CDataWrapper data_pack;
    CDataWrapper target_cdw;
    data_pack.addBoolValue("bv", (int32_t)0);
    data_pack.addInt32Value("i32v", (int32_t)0);
    data_pack.addInt64Value("i64v", (int64_t)0);
    data_pack.addDoubleValue("dbv", (double)36.6);
    const std::string json_serialization = data_pack.getJSONString();
    CDWUniquePtr deserialized = CDataWrapper::instanceFromJson(json_serialization);
    ASSERT_TRUE(deserialized->isDoubleValue("dbv"));
    ASSERT_TRUE(deserialized->isInt64Value("i64v"));
    ASSERT_TRUE(deserialized->isInt32Value("i32v"));
    ASSERT_TRUE(deserialized->isBoolValue("bv"));
    
    target_cdw.setSerializedJsonData(json_serialization.c_str());
    ASSERT_TRUE(target_cdw.isDoubleValue("dbv"));
    ASSERT_TRUE(target_cdw.isInt64Value("i64v"));
    ASSERT_TRUE(target_cdw.isInt32Value("i32v"));
    ASSERT_TRUE(target_cdw.isBoolValue("bv"));
}

TEST(CDataWrapperTest, MemoryLeaks) {
    int idx = 0;
    CDataWrapper data_pack;
    CDWUniquePtr deserialized;
    data_pack.addBoolValue("bv", (int32_t)0);
    data_pack.addInt32Value("i32v", (int32_t)0);
    data_pack.addInt64Value("i64v", (int64_t)0);
    data_pack.addDoubleValue("dbv", (double)36.6);
    for (; idx < 1000000; idx++) {
        const std::string json_serialization = data_pack.getJSONString();
        deserialized = CDataWrapper::instanceFromJson(json_serialization);
    }
    ASSERT_TRUE(deserialized->isDoubleValue("dbv"));
    ASSERT_TRUE(deserialized->isInt64Value("i64v"));
    ASSERT_TRUE(deserialized->isInt32Value("i32v"));
    ASSERT_TRUE(deserialized->isBoolValue("bv"));
}

TEST(CDataWrapperTest, Performance) {
    int idx = 0;
    CDataWrapper data_pack;
    CDWUniquePtr cloned;
    for (; idx < 1000000; idx++) {
        data_pack.addBoolValue("bv", (int32_t)0);
        data_pack.addInt32Value("i32v", (int32_t)0);
        data_pack.addInt64Value("i64v", (int64_t)0);
        data_pack.addDoubleValue("dbv", (double)36.6);
        cloned.reset(data_pack.clone());
        ASSERT_TRUE(cloned->isInt64Value("i64v"));
        ASSERT_TRUE(cloned->isInt32Value("i32v"));
        ASSERT_TRUE(cloned->isBoolValue("bv"));
        data_pack.reset();
    }
}

TEST(CDataWrapperTest, TestJsonDouble) {
    const char* test_json_translation="{\"double_key\":[1.0,2.1,-1.0,-0.9]}";
    double test_var[]={1.0,2.1,-1.0,-0.9};
    CDWUniquePtr data = CDataWrapper::instanceFromJson(test_json_translation);
    ASSERT_TRUE(data->hasKey("double_key"));
    ASSERT_TRUE(data->isVectorValue("double_key"));
    ChaosUniquePtr<CMultiTypeDataArrayWrapper> p(data->getVectorValue("double_key"));
    ASSERT_TRUE(p.get());

    for(int cnt=0;cnt<p->size();cnt++){
        ASSERT_EQ( test_var[cnt], p->getDoubleElementAtIndex(cnt));
    }
}
TEST(CDataWrapperTest, TestEmptyBSONToJSON) {
    CDataWrapper bson_a((const char*)NULL, 0);
    CDataWrapper bson_b((const char*)NULL);
    const std::string json_a = bson_a.getJSONString();
    const std::string can_json_a = bson_a.getCompliantJSONString();
    ASSERT_STREQ(json_a.c_str(), "{ }");
    ASSERT_STREQ(can_json_a.c_str(), "{ }");
    const std::string json_b = bson_b.getJSONString();
    const std::string can_json_b = bson_b.getCompliantJSONString();
    ASSERT_STREQ(json_b.c_str(), "{ }");
    ASSERT_STREQ(can_json_b.c_str(), "{ }");
}
TEST(CDataWrapperTest, TestEmptyJSONToBSON) {
    CDWUniquePtr bson_a = CDataWrapper::instanceFromJson("{}");
    CDWUniquePtr bson_b = CDataWrapper::instanceFromJson(std::string());
    const std::string json_a = bson_a->getJSONString();
    const std::string can_json_a = bson_a->getCompliantJSONString();
    ASSERT_STREQ(json_a.c_str(), "{ }");
    ASSERT_STREQ(can_json_a.c_str(), "{ }");
    const std::string json_b = bson_b->getJSONString();
    const std::string can_json_b = bson_b->getCompliantJSONString();
    ASSERT_STREQ(json_b.c_str(), "{ }");
    ASSERT_STREQ(can_json_b.c_str(), "{ }");
}
