/*
 *	CDataVariant.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 30/03/16 INFN, National Institute of Nuclear Physics
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

#include <chaos/common/data/CDataVariant.h>

using namespace chaos;
using namespace chaos::common::data;

CDataVariant::CDataVariant(int32_t int32_value):
type(DataType::TYPE_INT32){
    _internal_variant = int32_value;
}

CDataVariant::CDataVariant(int64_t int64_value):
type(DataType::TYPE_INT64){
    _internal_variant = int64_value;
}

CDataVariant::CDataVariant(double double_value):
type(DataType::TYPE_DOUBLE){
   _internal_variant = double_value;
}

CDataVariant::CDataVariant(bool bool_value):
type(DataType::TYPE_BOOLEAN){
    _internal_variant = bool_value;
}

CDataVariant::CDataVariant(const std::string& string_value):
type(DataType::TYPE_STRING) {
   _internal_variant = string_value;
}

CDataVariant::CDataVariant(const CDataBuffer& buffer_value):
type(DataType::TYPE_BYTEARRAY) {
    _internal_variant = buffer_value;
}

CDataVariant::CDataVariant(const CDataVariant& to_copy):
type(to_copy.type),
_internal_variant(to_copy._internal_variant){    
}

CDataVariant::CDataVariant():
type(DataType::TYPE_UNDEFINED),
_internal_variant(){
}

DataType::DataType CDataVariant::getType() const{
    return type;
}

int32_t CDataVariant::asInt32() const {
    return  boost::get<int32_t>(_internal_variant);
}
int64_t CDataVariant::asInt64() const {
    return  boost::get<int64_t>(_internal_variant);
}
double CDataVariant::asDouble() const {
    return  boost::get<double>(_internal_variant);
}
bool CDataVariant::asBool() const {
    return  boost::get<bool>(_internal_variant);
}
const std::string& CDataVariant::asString() const {
    return  boost::get<std::string>(_internal_variant);
}
const CDataBuffer& CDataVariant::asCDataBuffer() const {
    return  boost::get<CDataBuffer>(_internal_variant);
}