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
#include <chaos/common/exception/CException.h>
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
    const char* test_json_translation="{\"double_key\":[1.0,2.1,-1.0,-0.9],\"string_vector\":[\"ciao\",\"come stai\"]}";
    CDataWrapper dconcat,empty,pieno;
    CDWUniquePtr data = CDataWrapper::instanceFromJson(test_json_translation);
    dconcat.addCSDataValue("empty",empty);
    dconcat.addCSDataValue("notempty",*data.get());

    dconcat.addCSDataValue("empty2",empty);
    pieno.addInt64Value("ts1",(int64_t)1LL);
    pieno.addInt64Value("ts2",(int64_t)2LL);
    pieno.addInt64Value("ts3",(int64_t)3LL);
    pieno.addInt64Value("ts4",(int64_t)4LL);
    pieno.addDoubleValue("double_key",-2);
    pieno.addInt64Value("ts5",(int64_t)5LL);
    pieno.addInt64Value("ts6",(int64_t)6LL);
    pieno.addStringValue("string_empty","");
    pieno.addInt32Value("int_key",3);
    pieno.addStringValue("string_key","this is a long text that try to do some space on bson");
    pieno.addBoolValue("bool_key",true);
    pieno.addBoolValue("bool_key1",false);

    ASSERT_TRUE(pieno.getCompliantJSONString().size());
    dconcat.addCSDataValue("pieno",pieno);


    double test_var[]={1.0,2.1,-1.0,-0.9};
    ASSERT_TRUE(dconcat.hasKey("empty"));
    ASSERT_TRUE(dconcat.isCDataWrapperValue("empty2"));
    ASSERT_TRUE(dconcat.hasKey("empty2"));
    ASSERT_TRUE(dconcat.isCDataWrapperValue("empty"));
    ASSERT_TRUE(dconcat.hasKey("notempty"));
    ASSERT_TRUE(dconcat.isCDataWrapperValue("notempty"));
    ASSERT_TRUE(dconcat.isCDataWrapperValue("pieno"));

    CDWUniquePtr t(dconcat.getCSDataValue("notempty"));
    ASSERT_TRUE(t->isVectorValue("double_key"));

    ChaosUniquePtr<CMultiTypeDataArrayWrapper> p(t->getVectorValue("double_key"));
    ASSERT_TRUE(p.get());

    for(int cnt=0;cnt<p->size();cnt++){
        ASSERT_EQ( test_var[cnt], p->getDoubleElementAtIndex(cnt));
    }
    ASSERT_STREQ("{ \"empty\" : {  }, \"notempty\" : { \"double_key\" : [ 1.0, 2.1000000000000000888, -1.0, -0.9000000000000000222 ], \"string_vector\" : [ \"ciao\", \"come stai\" ] }, \"empty2\" : {  }, \"pieno\" : { \"ts1\" : 1, \"ts2\" : 2, \"ts3\" : 3, \"ts4\" : 4, \"double_key\" : -2.0, \"ts5\" : 5, \"ts6\" : 6, \"string_empty\" : \"\", \"int_key\" : 3, \"string_key\" : \"this is a long text that try to do some space on bson\", \"bool_key\" : true, \"bool_key1\" : false } }", dconcat.getCompliantJSONString().c_str());
}
TEST(CDataWrapperTest, DateToLong) {
    const char * json = "{  \"ndk_heartbeat\" : { \"$date\" : { \"$numberLong\" : \"1511968737899\" } } }";
    CDWUniquePtr data = CDataWrapper::instanceFromJson(json);
    ASSERT_EQ(data->getInt64Value("ndk_heartbeat"), 1511968737899);
}
TEST(CDataWrapperTest, SimpleStringNoException) {
    const char * json = "{\"powerOn\":\"true\"}";
    CDataWrapper data;
    data.setSerializedJsonData(json);
    ASSERT_EQ(0, 0);
}

TEST(CDataWrapperTest, SimpleStringException) {
    const char * json = "{\"powerOn\"::true\"}";
    CDataWrapper data;
    try{
      data.setSerializedJsonData(json);
      ASSERT_EQ(1, 0);
    } catch (chaos::CException e) {
      ASSERT_EQ(0, 0);
    }

}
