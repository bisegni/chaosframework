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
    CDataVariant str_v("35.5");
    CDataVariant neg_str_v("-35.5");
    CDataVariant double_v(35.5);
    CDataVariant neg_double_v(-35.5);
    
    //str variant
    ASSERT_EQ(static_cast<double>(str_v), 35.5);
    ASSERT_EQ(static_cast<int32_t>(str_v), 35);
    ASSERT_TRUE(static_cast<bool>(str_v));
    ASSERT_STREQ(static_cast< ChaosSharedPtr<CDataBuffer> >(str_v)->getBuffer(), "35.5");
    
    //neg str variant
    ASSERT_EQ(static_cast<double>(neg_str_v), -35.5);
    ASSERT_EQ(static_cast<int32_t>(neg_str_v), -35);
    ASSERT_TRUE(static_cast<bool>(neg_str_v));
    ASSERT_STREQ(static_cast< ChaosSharedPtr<CDataBuffer> >(neg_str_v)->getBuffer(), "-35.5");
    
    //double variant
    ASSERT_EQ(static_cast<double>(double_v), 35.5);
    ASSERT_EQ(static_cast<int32_t>(double_v), 35);
    ASSERT_TRUE(static_cast<bool>(double_v));
    
    //double neg variant
    ASSERT_EQ(static_cast<double>(neg_double_v), -35.5);
    ASSERT_EQ(static_cast<int32_t>(neg_double_v), -35);
    ASSERT_TRUE(static_cast<bool>(neg_double_v));
}
