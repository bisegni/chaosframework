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
#include <chaos/common/data/CDataWrapper.h>
#include <gtest/gtest.h>
#include <boost/lexical_cast.hpp>
using namespace chaos::common::data;

TEST(CDataWrapperTest, Normal) {
    CDataWrapper data_pack;
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
