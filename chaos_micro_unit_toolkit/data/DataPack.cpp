/*
 *	DataPack.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 02/08/2017 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
 */

#include <chaos_micro_unit_toolkit/data/DataPack.h>

using namespace chaos::micro_unit_toolkit;
using namespace chaos::micro_unit_toolkit::data;

ChaosUniquePtr<DataPack> DataPack::newFromBuffer(const char *data,
                                                 const size_t data_len,
                                                 bool *parsed) {
    ChaosUniquePtr<DataPack> new_dp(new DataPack());
    Json::Reader reader;
    bool parse_result = reader.parse(data,
                                     data+data_len,
                                     new_dp->root_json_object);
    if(parsed){*parsed = parse_result;}
    return new_dp;
}

DataPack::DataPack() {}

DataPack::DataPack(const Json::Value& src_root_json_object):
root_json_object(src_root_json_object){}

DataPack::~DataPack() {}

void DataPack::addBoolValue(const std::string& key, bool value) {
    root_json_object[key] = Json::Value(value);
}

const bool DataPack::getBool(const std::string& key) const {
    return root_json_object[key].asBool();
}

void DataPack::addInt32Value(const std::string& key, int32_t value) {
    root_json_object[key] = Json::Value(value);
}

const int32_t DataPack::getInt32Value(const std::string& key) const {
     return root_json_object[key].asInt();
}

void DataPack::addInt64Value(const std::string& key,
                             int64_t value) {
   root_json_object[key] = Json::Value(value);
}

const int64_t DataPack::getInt64Value(const std::string& key) const {
    return root_json_object[key].asInt64();
}

void DataPack::addDoubleValue(const std::string& key, double value) {
    root_json_object[key] = Json::Value(value);
}

const double DataPack::getDouble(const std::string& key) const {
    return root_json_object[key].asDouble();
}

void DataPack::addStringValue(const std::string& key, const std::string& value) {
    root_json_object[key] = Json::Value(value);
}

std::string DataPack::getString(const std::string& key) const {
    return root_json_object[key].asString();
}

void DataPack::addDataPackValue(const std::string& key, DataPack& value) {
    root_json_object[key] = value.root_json_object;
}

const ChaosUniquePtr<DataPack> DataPack::getDataPackValue(const std::string& key) const {
    return ChaosUniquePtr<DataPack>(new DataPack(root_json_object[key]));
}

void DataPack::createArrayForKey(const std::string& key) {
    root_json_object[key] = Json::Value(Json::ValueType::arrayValue);
}

void DataPack::appendBoolValue(const std::string& arr_key, bool value) {
    root_json_object[arr_key].append(Json::Value(value));
}

void DataPack::appendInt32Value(const std::string& arr_key, int32_t value) {
    root_json_object[arr_key].append(Json::Value(value));
}

void DataPack::appendInt64Value(const std::string& arr_key, int64_t value) {
    root_json_object[arr_key].append(Json::Value(value));
}

void DataPack::appendDoubleValue(const std::string& arr_key, double value) {
    root_json_object[arr_key].append(Json::Value(value));
}

void DataPack::appendStringValue(const std::string& arr_key, const std::string& value) {
    root_json_object[arr_key].append(Json::Value(value));
}

void DataPack::appendDataPackValue(const std::string& arr_key, DataPack& value) {
    root_json_object[arr_key].append(value.root_json_object);
}

std::string DataPack::toString() {
    Json::StyledWriter w;
    return w.write(root_json_object);
}
/*
 nullValue = 0, ///< 'null' value
 intValue,      ///< signed integer value
 uintValue,     ///< unsigned integer value
 realValue,     ///< double value
 stringValue,   ///< UTF-8 string value
 booleanValue,  ///< bool value
 arrayValue,    ///< array value (ordered list)
 objectValue    ///< object value (collection of name/value pairs).
*/
