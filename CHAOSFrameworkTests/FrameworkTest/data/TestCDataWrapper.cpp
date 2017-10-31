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
    ASSERT_EQ(data_pack.getBoolValue("bv"), false);
    ASSERT_EQ(data_pack.getInt32Value("i32v"), 0);
    ASSERT_EQ(data_pack.getInt64Value("i64v"), 0);
    ASSERT_EQ(data_pack.getDoubleValue("dbv"), 36.6);
}

