//
//  HealtMetric.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 06/05/15.
//  Copyright (c) 2015 INFN. All rights reserved.
//

#include <chaos/common/healt_system/HealtMetric.h>
using namespace chaos::common::data;
using namespace chaos::common::healt_system;

HealtMetric::HealtMetric(const std::string& _name,
                         const chaos::DataType::DataType _type):
name(_name),
type(_type){}
void HealtMetric::addMetricToCD(CDataWrapper *data) {
    if(data) addMetricToCD(*data);
}


BoolHealtMetric::BoolHealtMetric(const std::string& _name):
HealtMetric(_name,
            chaos::DataType::TYPE_BOOLEAN),
value(false){}
void BoolHealtMetric::addMetricToCD(CDataWrapper& data) {
    data.addBoolValue(name.c_str(), value);
}

Int32HealtMetric::Int32HealtMetric(const std::string& _name):
HealtMetric(_name,
            chaos::DataType::TYPE_INT32),
value(0){}
void Int32HealtMetric::addMetricToCD(CDataWrapper& data) {
    data.addInt32Value(name, value);
}

Int64HealtMetric::Int64HealtMetric(const std::string& _name):
HealtMetric(_name,
            chaos::DataType::TYPE_INT64),
value(0){}
void Int64HealtMetric::addMetricToCD(CDataWrapper& data) {
    data.addInt64Value(name, value);
}


DoubleHealtMetric::DoubleHealtMetric(const std::string& _name):
HealtMetric(_name,
            chaos::DataType::TYPE_DOUBLE),
value(0.0){}
void DoubleHealtMetric::addMetricToCD(CDataWrapper& data) {
    data.addDoubleValue(name, value);
}

StringHealtMetric::StringHealtMetric(const std::string& _name):
HealtMetric(_name,
            chaos::DataType::TYPE_STRING),
value(""){}
void StringHealtMetric::addMetricToCD(CDataWrapper& data) {
    data.addStringValue(name, value);
}
