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
    CDWUniquePtr json_deserialized;
    CDWUniquePtr bson_deserialized;
    data_pack.addBoolValue("bv", (int32_t)0);
    data_pack.addInt32Value("i32v", (int32_t)0);
    data_pack.addInt64Value("i64v", (int64_t)0);
    data_pack.addDoubleValue("dbv", (double)36.6);
    data_pack.appendInt32ToArray(1);
    data_pack.appendInt64ToArray(2);
    data_pack.appendDoubleToArray(3.0);
    data_pack.appendStringToArray("array_lement");
    data_pack.finalizeArrayForKey("array");
    const std::string json_serialization = data_pack.getJSONString();
    for (; idx < 1000000; idx++) {
        bson_deserialized = CDWUniquePtr(new CDataWrapper(data_pack.getBSONRawData(), data_pack.getBSONRawSize()));
        ASSERT_TRUE(bson_deserialized.get());
        json_deserialized = CDataWrapper::instanceFromJson(json_serialization);
        ASSERT_TRUE(json_deserialized.get());
        ASSERT_STREQ(json_deserialized->toHash().c_str(), bson_deserialized->toHash().c_str());
        ASSERT_TRUE(json_deserialized->isDoubleValue("dbv"));
        ASSERT_TRUE(json_deserialized->isInt64Value("i64v"));
        ASSERT_TRUE(json_deserialized->isInt32Value("i32v"));
        ASSERT_TRUE(json_deserialized->isBoolValue("bv"));
        
        ChaosUniquePtr<CMultiTypeDataArrayWrapper> array_ptr(json_deserialized->getVectorValue("array"));
        ASSERT_EQ(array_ptr->size(), 4);
        ASSERT_EQ(array_ptr->getInt32ElementAtIndex(0), 1);
        ASSERT_EQ(array_ptr->getInt64ElementAtIndex(1), 2);
        ASSERT_EQ(array_ptr->getDoubleElementAtIndex(2), 3.0);
        ASSERT_STREQ(array_ptr->getStringElementAtIndex(3).c_str(), "array_lement");
    }
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

TEST(CDataWrapperTest, TestConcatenation) {
    const char* test_json_translation="{\"double_key\":[1.0,2.1,-1.0,-0.9]}";
    CDataWrapper dconcat,empty;
    CDWUniquePtr data = CDataWrapper::instanceFromJson(test_json_translation);
    dconcat.addCSDataValue("empty",empty);
    dconcat.addCSDataValue("notempty",*data.get());
    dconcat.addCSDataValue("empty2",empty);
    double test_var[]={1.0,2.1,-1.0,-0.9};
    ASSERT_TRUE(dconcat.hasKey("empty"));
    ASSERT_TRUE(dconcat.isCDataWrapperValue("empty2"));
    ASSERT_TRUE(dconcat.hasKey("empty2"));
    ASSERT_TRUE(dconcat.isCDataWrapperValue("empty"));
    ASSERT_TRUE(dconcat.hasKey("notempty"));
    ASSERT_TRUE(dconcat.isCDataWrapperValue("notempty"));
    CDWUniquePtr t(dconcat.getCSDataValue("notempty"));
    ASSERT_TRUE(t->isVectorValue("double_key"));

    ChaosUniquePtr<CMultiTypeDataArrayWrapper> p(t->getVectorValue("double_key"));
    ASSERT_TRUE(p.get());

    for(int cnt=0;cnt<p->size();cnt++){
        ASSERT_EQ( test_var[cnt], p->getDoubleElementAtIndex(cnt));
    }
    ASSERT_STREQ("{ \"empty\" : {  }, \"notempty\" : { \"double_key\" : [ 1.0, 2.1000000000000000888, -1.0, -0.9000000000000000222 ] }, \"empty2\" : {  } }", dconcat.getCompliantJSONString().c_str());
}
