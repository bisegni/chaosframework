//
//  main.cpp
//  TestProperty
//
//  Created by bisegni on 30/08/2017.
//  Copyright © 2017 bisegni. All rights reserved.
//

#include <chaos/common/property/property.h>
#include <iostream>
#include <assert.h>
using namespace chaos::common::data;
using namespace chaos::common::property;

unsigned int ch_count = 0;
unsigned int uh_count = 0;

bool changeHandler(const std::string& group_name,
                   const std::string& property_name,
                   const CDataVariant& property_value) {
    ch_count++;
    //std::cout << "changeHandler:" << property_value.asString();
    return true;
}

bool updateHandler(const std::string& group_name,
                   const std::string& property_name,
                   const CDataVariant& old_value,
                   const CDataVariant& property_value) {
    uh_count++;
    //std::cout << "updateHandler:" << old_value.asString() << "-" << property_value.asString();
    return true;
}

int main(int argc, const char * argv[]) {
    PropertyGroup pg_1("property_group_1");
    PropertyGroup pg_2("property_group_2");
    pg_2.setPropertyValueChangeFunction(changeHandler);
    pg_2.setPropertyValueUpdatedFunction(updateHandler);
    
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
    
    PropertyGroupSDWrapper pg_w1(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(PropertyGroup, pg_1));
    PropertyGroupSDWrapper pg_w2(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(PropertyGroup, pg_2));
    std::string pg_w1_ser = pg_w1.serialize()->getJSONString();
    std::string pg_w2_ser = pg_w2.serialize()->getJSONString();
    assert(ch_count == 4);
    assert(uh_count == 4);
    assert(pg_1.getPropertyValue("prop_1").getType() == chaos::DataType::TYPE_INT32 &&
           pg_2.getPropertyValue("prop_1").getType() == chaos::DataType::TYPE_INT32);
    assert(pg_1.getPropertyValue("prop_2").getType() == chaos::DataType::TYPE_INT64 &&
           pg_2.getPropertyValue("prop_2").getType() == chaos::DataType::TYPE_INT64);
    assert(pg_1.getPropertyValue("prop_3").getType() == chaos::DataType::TYPE_DOUBLE &&
           pg_2.getPropertyValue("prop_3").getType() == chaos::DataType::TYPE_DOUBLE);
    assert(pg_1.getPropertyValue("prop_4").getType() == chaos::DataType::TYPE_BOOLEAN &&
           pg_2.getPropertyValue("prop_4").getType() == chaos::DataType::TYPE_BOOLEAN);
    return 0;
}
