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

#include <chaos_micro_unit_toolkit/data/DataPack.h>
#include <chaos_micro_unit_toolkit/external_lib/base64.h>

using namespace Json;
using namespace chaos::micro_unit_toolkit;
using namespace chaos::micro_unit_toolkit::data;

DataPackUniquePtr DataPack::newFromBuffer(const char *data,
                                          const size_t data_len,
                                          bool *parsed) {
    DataPackUniquePtr new_dp(new DataPack());
    Reader reader;
    bool parse_result = reader.parse(data,
                                     data+data_len,
                                     new_dp->root_json_object);
    if(parsed){*parsed = parse_result;}
    return new_dp;
}

DataPack::DataPack() {}

DataPack::DataPack(const Value& src_root_json_object):
root_json_object(src_root_json_object){}

DataPack::~DataPack() {}

bool DataPack::hasKey(const std::string& key) {
    return root_json_object.isMember(key);
}

void DataPack::addBool(const std::string& key, bool value) {
    root_json_object[key] = Value(value);
}
const bool DataPack::isBool(const std::string& key) const {
    return root_json_object[key].isBool();
}

const bool DataPack::getBool(const std::string& key) const {
    return root_json_object[key].asBool();
}
void DataPack::addInt32(const std::string& key,
                        int32_t value) {
    root_json_object[key] = Value(value);
}
const bool DataPack::isInt32(const std::string& key) const {
    return root_json_object[key].isInt();
}
const int32_t DataPack::getInt32(const std::string& key) const {
    return root_json_object[key].asInt();
}
void DataPack::addInt64(const std::string& key,
                        int64_t value) {
    root_json_object[key] = Value(value);
}
const bool DataPack::isInt64(const std::string& key) const {
    return root_json_object[key].isInt64();
}
const int64_t DataPack::getInt64(const std::string& key) const {
    return root_json_object[key].asInt64();
}
void DataPack::addDouble(const std::string& key,
                         double value) {
    root_json_object[key] = Value(value);
}
const bool DataPack::isDouble(const std::string& key) const {
    return root_json_object[key].isDouble();
}
const double DataPack::getDouble(const std::string& key) const {
    return root_json_object[key].asDouble();
}
void DataPack::addString(const std::string& key,
                         const std::string& value) {
    root_json_object[key] = Value(value);
}
const bool DataPack::isString(const std::string& key) const {
    return root_json_object[key].isString();
}
std::string DataPack::getString(const std::string& key) const {
    return root_json_object[key].asString();
}
void DataPack::addDataPack(const std::string& key,
                           DataPack& value) {
    root_json_object[key] = value.root_json_object;
}
const bool DataPack::isDataPack(const std::string& key) const {
    return root_json_object[key].isObject();
}
DataPackUniquePtr DataPack::getDataPack(const std::string& key) const {
    return DataPackUniquePtr(new DataPack(root_json_object[key]));
}

void DataPack::addBinary(const std::string& key,
                         const char *s,
                         const unsigned int len) {
    root_json_object[key] = Value(base64_encode(reinterpret_cast<const unsigned char*>(s), len));
}
std::string DataPack::getBinary(const std::string& key) {
    return base64_decode(root_json_object[key].asString());
}

void DataPack::createArrayForKey(const std::string& key) {
    root_json_object[key] = Value(arrayValue);
}
const bool DataPack::isArray(const std::string& key) const {
    return root_json_object[key].isArray();
}
void DataPack::appendBool(const std::string& arr_key,
                          bool value) {
    root_json_object[arr_key].append(Value(value));
}

void DataPack::appendInt32(const std::string& arr_key,
                           int32_t value) {
    root_json_object[arr_key].append(Value(value));
}

void DataPack::appendInt64(const std::string& arr_key,
                           int64_t value) {
    root_json_object[arr_key].append(Value(value));
}

void DataPack::appendDouble(const std::string& arr_key,
                            double value) {
    root_json_object[arr_key].append(Value(value));
}

void DataPack::appendString(const std::string& arr_key,
                            const std::string& value) {
    root_json_object[arr_key].append(Value(value));
}

void DataPack::appendDataPack(const std::string& arr_key, DataPack& value) {
    root_json_object[arr_key].append(value.root_json_object);
}

std::string DataPack::toString() {
    StyledWriter w;
    return w.write(root_json_object);
}

std::string DataPack::toUnformattedString() {
    FastWriter w;
    return w.write(root_json_object);
}
