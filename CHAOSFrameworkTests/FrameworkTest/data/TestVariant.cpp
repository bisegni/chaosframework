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

#include <chaos/common/data/CDataVariant.h>
#include <gtest/gtest.h>

using namespace chaos::common::data;

TEST(VariantTest, Normal) {
    std::string tmp_str;
    CDataVariant str_v("36.6");
    CDataVariant neg_str_v("-36.6");
    CDataVariant double_v(36.6);
    CDataVariant neg_double_v(-36.6);
    
    //str variant
    ASSERT_EQ(static_cast<double>(str_v), 36.6);
    ASSERT_EQ(static_cast<int32_t>(str_v), 36);
    ASSERT_TRUE(static_cast<bool>(str_v));
    tmp_str.assign(static_cast< ChaosSharedPtr<CDataBuffer> >(str_v)->getBuffer(), static_cast< ChaosSharedPtr<CDataBuffer> >(str_v)->getBufferSize());
    ASSERT_STREQ(tmp_str.c_str(), "36.6");
    
    //neg str variant
    ASSERT_EQ(static_cast<double>(neg_str_v), -36.6);
    ASSERT_EQ(static_cast<int32_t>(neg_str_v), -36);
    ASSERT_TRUE(static_cast<bool>(neg_str_v));
    tmp_str.assign(static_cast< ChaosSharedPtr<CDataBuffer> >(neg_str_v)->getBuffer(), static_cast< ChaosSharedPtr<CDataBuffer> >(neg_str_v)->getBufferSize());
    ASSERT_STREQ(tmp_str.c_str(), "-36.6");
    
    //double variant
    ASSERT_STREQ(double_v.asString(2).c_str(), "36.60");
    ASSERT_STREQ(double_v.asString().c_str(), "36.6");
    ASSERT_EQ(static_cast<int32_t>(double_v), 36);
    ASSERT_TRUE(static_cast<bool>(double_v));
    
    //double neg variant
    ASSERT_STREQ(neg_double_v.asString(2).c_str(), "-36.60");
    ASSERT_STREQ(neg_double_v.asString().c_str(), "-36.6");
    ASSERT_EQ(static_cast<int32_t>(neg_double_v), -36);
    ASSERT_TRUE(static_cast<bool>(neg_double_v));
}
