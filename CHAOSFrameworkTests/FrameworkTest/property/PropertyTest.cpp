/*
 * Copyright 2012, 2017 INFN
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

#include "PropertyTest.h"
using namespace chaos::common::data;
using namespace chaos::common::property;

PropertyTest::PropertyTest():
ch_count(0),
uh_count(0){}

bool PropertyTest::changeHandler(const std::string& group_name,
                   const std::string& property_name,
                   const CDataVariant& property_value) {
    ch_count++;
    return true;
}

bool PropertyTest::updateHandler(const std::string& group_name,
                   const std::string& property_name,
                   const CDataVariant& old_value,
                   const CDataVariant& property_value) {
    uh_count++;
    return true;
}

void PropertyTest::SetUp() {
    ch_count = 0;
    uh_count = 0;
}

TEST_F(PropertyTest, PropertyWholeTest) {
    PropertyGroup pg_1("property_group_1");
    PropertyGroup pg_2("property_group_2");
    pg_2.setPropertyValueChangeFunction(ChaosBind(&PropertyTest::changeHandler, this, ChaosBindPlaceholder(_1), ChaosBindPlaceholder(_2), ChaosBindPlaceholder(_3)));
    pg_2.setPropertyValueUpdatedFunction(ChaosBind(&PropertyTest::updateHandler, this, ChaosBindPlaceholder(_1), ChaosBindPlaceholder(_2), ChaosBindPlaceholder(_3), ChaosBindPlaceholder(_4)));
    
    pg_1.addProperty("prop_1", "prop_1_desc", chaos::DataType::TYPE_INT32);
    pg_1.addProperty("prop_2", "prop_2_desc", chaos::DataType::TYPE_INT64);
    pg_1.addProperty("prop_3", "prop_3_desc", chaos::DataType::TYPE_DOUBLE);
    pg_1.addProperty("prop_4", "prop_4_desc", chaos::DataType::TYPE_BOOLEAN);
    
    pg_1.setPropertyValue("prop_1", CDataVariant((int32_t)1));
    pg_1.setPropertyValue("prop_2", CDataVariant((int64_t)2));
    pg_1.setPropertyValue("prop_3", CDataVariant((double)3.0));
    pg_1.setPropertyValue("prop_4", CDataVariant((bool)true));
    
    pg_2.copyPropertiesFromGroup(pg_1);
    pg_2.updatePropertiesValueFromSourceGroup(pg_1);
    
    ASSERT_EQ(4, ch_count);
    ASSERT_EQ(4, uh_count);
    ASSERT_EQ(pg_1.getPropertyValue("prop_1").getType(), chaos::DataType::TYPE_INT32);
    ASSERT_EQ(pg_2.getPropertyValue("prop_1").getType(), chaos::DataType::TYPE_INT32);
    ASSERT_EQ(pg_1.getPropertyValue("prop_2").getType(), chaos::DataType::TYPE_INT64);
    ASSERT_EQ(pg_2.getPropertyValue("prop_2").getType(), chaos::DataType::TYPE_INT64);
    ASSERT_EQ(pg_1.getPropertyValue("prop_3").getType(), chaos::DataType::TYPE_DOUBLE);
    ASSERT_EQ(pg_2.getPropertyValue("prop_3").getType(), chaos::DataType::TYPE_DOUBLE);
    ASSERT_EQ(pg_1.getPropertyValue("prop_4").getType(), chaos::DataType::TYPE_BOOLEAN);
    ASSERT_EQ(pg_2.getPropertyValue("prop_4").getType(), chaos::DataType::TYPE_BOOLEAN);
}
