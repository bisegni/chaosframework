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

#include <chaos/common/data/CDataVariant.h>
#include <chaos/common/exception/CException.h>

#include <sstream>

using namespace chaos;
using namespace chaos::common::data;

#define SAFE_LEXICAL_WITH_DEFAULT(x, d)\
try{return x;}catch(...){return d;}

#define SAFE_STREAM_CONV(t, s, v)\
t v;\
std::istringstream ss(s);\
ss >> v;

#pragma mark boolvisitor
bool bool_visitor::operator()(const bool bv) const {return bv;}
bool bool_visitor::operator()(const int32_t i32v) const {return static_cast<bool>(i32v);}
bool bool_visitor::operator()(const uint32_t ui32v) const {return static_cast<bool>(ui32v);}
bool bool_visitor::operator()(const int64_t i64v) const {return static_cast<bool>(i64v);}
bool bool_visitor::operator()(const uint64_t ui64v) const {return static_cast<bool>(ui64v);}
bool bool_visitor::operator()(const double dv) const {return static_cast<bool>(dv);}
bool bool_visitor::operator()(const std::string& str) const {SAFE_STREAM_CONV(bool, str, b); return b;}
bool bool_visitor::operator()(const ChaosSharedPtr<CDataBuffer>& buffer) const {return static_cast<int32_t>(buffer->getBufferSize());}
bool bool_visitor::operator()(const ChaosSharedPtr<CDataWrapper>& buffer) const {return buffer.get();}

#pragma mark i32visitor
int32_t int32_t_visitor::operator()(const bool bv) const {return static_cast<int32_t>(bv);}
int32_t int32_t_visitor::operator()(const int32_t i32v) const {return i32v;}
int32_t int32_t_visitor::operator()(const uint32_t ui32v) const {return static_cast<int32_t>(ui32v);}
int32_t int32_t_visitor::operator()(const int64_t i64v) const {return static_cast<int32_t>(i64v);}
int32_t int32_t_visitor::operator()(const uint64_t ui64v) const {return static_cast<int32_t>(ui64v);}
int32_t int32_t_visitor::operator()(const double dv) const {return static_cast<int32_t>(dv);}
int32_t int32_t_visitor::operator()(const std::string& str) const {SAFE_STREAM_CONV(int32_t, str, i32); return i32;}
int32_t int32_t_visitor::operator()(const ChaosSharedPtr<CDataBuffer>& buffer) const {return static_cast<int32_t>(buffer->getBufferSize());}
int32_t int32_t_visitor::operator()(const ChaosSharedPtr<CDataWrapper>& buffer) const {return static_cast<int32_t>(buffer->getBSONRawSize());}

#pragma mark ui32visitor
uint32_t uint32_t_visitor::operator()(const bool bv) const {return static_cast<uint32_t>(bv);}
uint32_t uint32_t_visitor::operator()(const int32_t i32v) const {return static_cast<uint32_t>(i32v);}
uint32_t uint32_t_visitor::operator()(const uint32_t ui32v) const {return ui32v;}
uint32_t uint32_t_visitor::operator()(const int64_t i64v) const {return static_cast<uint32_t>(i64v);}
uint32_t uint32_t_visitor::operator()(const uint64_t ui64v) const {return static_cast<uint32_t>(ui64v);}
uint32_t uint32_t_visitor::operator()(const double dv) const {return static_cast<uint32_t>(dv);}
uint32_t uint32_t_visitor::operator()(const std::string& str) const {SAFE_STREAM_CONV(int32_t, str, i32); return static_cast<uint32_t>(i32);}
uint32_t uint32_t_visitor::operator()(const ChaosSharedPtr<CDataBuffer>& buffer) const {return buffer->getBufferSize();}
uint32_t uint32_t_visitor::operator()(const ChaosSharedPtr<CDataWrapper>& buffer) const {return static_cast<uint32_t>(buffer->getBSONRawSize());}

#pragma mark i64visitor
int64_t int64_t_visitor::operator()(const bool bv) const {return static_cast<int64_t>(bv);}
int64_t int64_t_visitor::operator()(const int32_t i32v) const {return static_cast<int64_t>(i32v);}
int64_t int64_t_visitor::operator()(const uint32_t ui32v) const {return static_cast<int64_t>(ui32v);}
int64_t int64_t_visitor::operator()(const int64_t i64v) const {return i64v;}
int64_t int64_t_visitor::operator()(const uint64_t ui64v) const {return static_cast<int64_t>(ui64v);}
int64_t int64_t_visitor::operator()(const double dv) const {return static_cast<int64_t>(dv);}
int64_t int64_t_visitor::operator()(const std::string& str) const {SAFE_STREAM_CONV(int64_t, str, i64); return i64;}
int64_t int64_t_visitor::operator()(const ChaosSharedPtr<CDataBuffer>& buffer) const {return static_cast<int64_t>(buffer->getBufferSize());}
int64_t int64_t_visitor::operator()(const ChaosSharedPtr<CDataWrapper>& buffer) const {return static_cast<int64_t>(buffer->getBSONRawSize());}

#pragma mark ui64visitor
uint64_t uint64_t_visitor::operator()(const bool bv) const {return static_cast<uint64_t>(bv);}
uint64_t uint64_t_visitor::operator()(const int32_t i32v) const {return static_cast<uint64_t>(i32v);}
uint64_t uint64_t_visitor::operator()(const uint32_t ui32v) const {return static_cast<uint64_t>(ui32v);}
uint64_t uint64_t_visitor::operator()(const int64_t i64v) const {return static_cast<uint64_t>(i64v);}
uint64_t uint64_t_visitor::operator()(const uint64_t ui64v) const {return ui64v;}
uint64_t uint64_t_visitor::operator()(const double dv) const {return static_cast<uint64_t>(dv);}
uint64_t uint64_t_visitor::operator()(const std::string& str) const {SAFE_STREAM_CONV(int64_t, str, i64); return static_cast<uint64_t>(i64);}
uint64_t uint64_t_visitor::operator()(const ChaosSharedPtr<CDataBuffer>& buffer) const {return (uint64_t)buffer->getBufferSize();}
uint64_t uint64_t_visitor::operator()(const ChaosSharedPtr<CDataWrapper>& buffer) const {return static_cast<uint64_t>(buffer->getBSONRawSize());}


#pragma mark doublevisitor
double double_visitor::operator()(const bool bv) const {return static_cast<double>(bv);}
double double_visitor::operator()(const int32_t i32v) const {return static_cast<double>(i32v);}
double double_visitor::operator()(const uint32_t ui32v) const {return static_cast<double>(ui32v);}
double double_visitor::operator()(const int64_t i64v) const {return static_cast<double>(i64v);}
double double_visitor::operator()(const uint64_t ui64v) const {return static_cast<double>(ui64v);}
double double_visitor::operator()(const double dv) const {return dv;}
double double_visitor::operator()(const std::string& str) const {SAFE_STREAM_CONV(double, str, d); return d;}
double double_visitor::operator()(const ChaosSharedPtr<CDataBuffer>& buffer) const {return static_cast<double>(buffer->getBufferSize());}
double double_visitor::operator()(const ChaosSharedPtr<CDataWrapper>& buffer) const {return static_cast<double>(buffer->getBSONRawSize());}

#pragma mark stringvisitor
std::string string_visitor::operator()(const bool bv) const {SAFE_LEXICAL_WITH_DEFAULT(boost::lexical_cast<std::string>(bv), "false")}
std::string string_visitor::operator()(const int32_t i32v) const {SAFE_LEXICAL_WITH_DEFAULT(boost::lexical_cast<std::string>(i32v), "0")}
std::string string_visitor::operator()(const uint32_t ui32v) const {SAFE_LEXICAL_WITH_DEFAULT(boost::lexical_cast<std::string>(ui32v), "0")}
std::string string_visitor::operator()(const int64_t i64v) const {SAFE_LEXICAL_WITH_DEFAULT(boost::lexical_cast<std::string>(i64v), "0")}
std::string string_visitor::operator()(const uint64_t ui64v) const {SAFE_LEXICAL_WITH_DEFAULT(boost::lexical_cast<std::string>(ui64v), "0")}
std::string string_visitor::operator()(const double dv) const {
    std::ostringstream oss;
    if(param>=0) {
        oss << std::fixed << std::setprecision(param);
    }
    oss << dv;
    return oss.str();
}
std::string string_visitor::operator()(const std::string& str) const {return str;}
std::string string_visitor::operator()(const ChaosSharedPtr<CDataBuffer>& buffer) const {return std::string(buffer->getBuffer(), buffer->getBufferSize());}
std::string string_visitor::operator()(const ChaosSharedPtr<CDataWrapper>& buffer) const {return buffer->getJSONString();}


#pragma mark CDataBuffervisitor
ChaosSharedPtr<CDataBuffer> CDataBuffer_visitor::operator()(const bool bv) const {return ChaosSharedPtr<CDataBuffer>(new CDataBuffer((const char*)&bv,sizeof(bool)));}
ChaosSharedPtr<CDataBuffer> CDataBuffer_visitor::operator()(const int32_t i32v) const {return ChaosSharedPtr<CDataBuffer>(new CDataBuffer((const char*)&i32v,sizeof(int32_t)));}
ChaosSharedPtr<CDataBuffer> CDataBuffer_visitor::operator()(const uint32_t ui32v) const {return ChaosSharedPtr<CDataBuffer>(new CDataBuffer((const char*)&ui32v,sizeof(uint32_t)));}
ChaosSharedPtr<CDataBuffer> CDataBuffer_visitor::operator()(const int64_t i64v) const {return ChaosSharedPtr<CDataBuffer>(new CDataBuffer((const char*)&i64v,sizeof(int64_t)));}
ChaosSharedPtr<CDataBuffer> CDataBuffer_visitor::operator()(const uint64_t ui64v) const {return ChaosSharedPtr<CDataBuffer>(new CDataBuffer((const char*)&ui64v,sizeof(uint64_t)));}
ChaosSharedPtr<CDataBuffer> CDataBuffer_visitor::operator()(const double dv) const {return ChaosSharedPtr<CDataBuffer>(new CDataBuffer((const char*)&dv,sizeof(double)));}
ChaosSharedPtr<CDataBuffer> CDataBuffer_visitor::operator()(const std::string& str) const {return ChaosSharedPtr<CDataBuffer>(new CDataBuffer(str.c_str(), (uint32_t)str.size()));}
ChaosSharedPtr<CDataBuffer> CDataBuffer_visitor::operator()(const ChaosSharedPtr<CDataBuffer>& buffer) const {return buffer;}
ChaosSharedPtr<CDataBuffer> CDataBuffer_visitor::operator()(const ChaosSharedPtr<CDataWrapper>& buffer) const {return ChaosSharedPtr<CDataBuffer>(new CDataBuffer(buffer->getJSONString().c_str(), (uint32_t)buffer->getJSONString().size()));}

#pragma mark CDataWrappervisitor
ChaosSharedPtr<CDataWrapper> CDataWrapper_visitor::operator()(const bool bv) const {throw CFatalException(-1,"invalid conversion to CDataWrapper from bool",__PRETTY_FUNCTION__);return ChaosSharedPtr<CDataWrapper>(new CDataWrapper());}
ChaosSharedPtr<CDataWrapper> CDataWrapper_visitor::operator()(const int32_t i32v) const {throw CFatalException(-1,"invalid conversion to CDataWrapper from int32_t",__PRETTY_FUNCTION__);return ChaosSharedPtr<CDataWrapper>(new CDataWrapper());}
ChaosSharedPtr<CDataWrapper> CDataWrapper_visitor::operator()(const uint32_t ui32v) const{throw CFatalException(-1,"invalid conversion to CDataWrapper from uint32_t",__PRETTY_FUNCTION__);return ChaosSharedPtr<CDataWrapper>(new CDataWrapper());}
ChaosSharedPtr<CDataWrapper> CDataWrapper_visitor::operator()(const int64_t i64v) const{throw CFatalException(-1,"invalid conversion to CDataWrapper from int64_t",__PRETTY_FUNCTION__);return ChaosSharedPtr<CDataWrapper>(new CDataWrapper());}
ChaosSharedPtr<CDataWrapper> CDataWrapper_visitor::operator()(const uint64_t ui64v) const {throw CFatalException(-1,"invalid conversion to CDataWrapper from uint64_t",__PRETTY_FUNCTION__);return ChaosSharedPtr<CDataWrapper>(new CDataWrapper());}
ChaosSharedPtr<CDataWrapper> CDataWrapper_visitor::operator()(const double dv) const {throw CFatalException(-1,"invalid conversion to CDataWrapper from double",__PRETTY_FUNCTION__);return ChaosSharedPtr<CDataWrapper>(new CDataWrapper());}
ChaosSharedPtr<CDataWrapper> CDataWrapper_visitor::operator()(const std::string& str) const {return ChaosSharedPtr<CDataWrapper>(new CDataWrapper(str.c_str()));}
ChaosSharedPtr<CDataWrapper> CDataWrapper_visitor::operator()(const ChaosSharedPtr<CDataBuffer>& buffer) const {return ChaosSharedPtr<CDataWrapper>(new CDataWrapper(buffer->getBuffer()));}
ChaosSharedPtr<CDataWrapper> CDataWrapper_visitor::operator()(const ChaosSharedPtr<CDataWrapper>& buffer) const {return ChaosSharedPtr<CDataWrapper>(new CDataWrapper(buffer->getBSONData()->getBufferPtr()));}


#pragma mark CDatavariant implementation
CDataVariant::CDataVariant(int32_t int32_value):
type(DataType::TYPE_INT32),
_internal_variant(int32_value) { }

CDataVariant::CDataVariant(uint32_t int32_value):
type(DataType::TYPE_INT32),
_internal_variant(int32_value) { }

CDataVariant::CDataVariant(int64_t int64_value):
type(DataType::TYPE_INT64),
_internal_variant(int64_value) { }

CDataVariant::CDataVariant(uint64_t int64_value):
type(DataType::TYPE_INT64),
_internal_variant(int64_value) { }

CDataVariant::CDataVariant(double double_value):
type(DataType::TYPE_DOUBLE),
_internal_variant(double_value) { }

CDataVariant::CDataVariant(bool bool_value):
type(DataType::TYPE_BOOLEAN),
_internal_variant(bool_value) { }

CDataVariant::CDataVariant(const std::string& string_value):_internal_variant(string_value){
    CDWUniquePtr tmp = CDataWrapper::instanceFromJson(string_value);
    if(tmp->isEmpty()) {
        type=DataType::TYPE_STRING;
    } else {
        type=DataType::TYPE_CLUSTER;
    }
}

CDataVariant::CDataVariant(const char * string_value):_internal_variant(std::string(string_value)){
    CDWUniquePtr tmp = CDataWrapper::instanceFromJson(string_value);
    if(tmp->isEmpty()) {
        type=DataType::TYPE_STRING;
    } else {
        type=DataType::TYPE_CLUSTER;
    }
}

CDataVariant::CDataVariant(CDBufferUniquePtr buffer_value):
type(DataType::TYPE_BYTEARRAY),
_internal_variant(ChaosSharedPtr<CDataBuffer>(buffer_value.release())) { }

CDataVariant::CDataVariant(CDataWrapper *buffer_value):
type(DataType::TYPE_CLUSTER),
_internal_variant(ChaosSharedPtr<CDataWrapper>(buffer_value)) { }

CDataVariant::CDataVariant(const CDataVariant& to_copy):
type(to_copy.type),
_internal_variant(to_copy._internal_variant) { }

CDataVariant::CDataVariant():
type(DataType::TYPE_UNDEFINED),
_internal_variant() { }

CDataVariant::CDataVariant(DataType::DataType _type,
                           const void *_value_pointer,
                           uint32_t _value_size):
type(_type){
    switch (type) {
        case DataType::TYPE_BOOLEAN:{
            _internal_variant = *static_cast<const bool*>(_value_pointer);
            break;
        }
        case DataType::TYPE_INT32:{
            _internal_variant = *static_cast<const int32_t*>(_value_pointer);
            break;
        }
        case DataType::TYPE_INT64:{
            _internal_variant = *static_cast<const int64_t*>(_value_pointer);
            break;
        }
        case DataType::TYPE_DOUBLE:{
            _internal_variant = *static_cast<const double*>(_value_pointer);
            break;
        }
        case DataType::TYPE_CLUSTER:{
            CDataWrapper*tmp=new CDataWrapper();
            tmp->setSerializedJsonData(static_cast<const char*>(_value_pointer));
            _internal_variant = ChaosSharedPtr<CDataWrapper>(tmp);
            break;
        }
        case DataType::TYPE_STRING:{
            _internal_variant = std::string(static_cast<const char*>(_value_pointer),
                                            _value_size);
            break;
        }
        case DataType::TYPE_BYTEARRAY:{
            _internal_variant = ChaosSharedPtr<CDataBuffer>(new CDataBuffer(static_cast<const char*>(_value_pointer),
                                                                            _value_size));
            break;
        }
        default:
            break;
    }
}

DataType::DataType CDataVariant::getType() const{
    return type;
}

bool CDataVariant::isValid() const{
    return (type != DataType::TYPE_UNDEFINED);
}

CDataVariant& CDataVariant::operator=(const CDataVariant& arg) {
    type = arg.type;
    _internal_variant = arg._internal_variant;
    return *this;
}

int32_t CDataVariant::asInt32() const {
    return  boost::apply_visitor( int32_t_visitor(), _internal_variant );
}

CDataVariant::operator int32_t() const {
    return asInt32();
}

uint32_t CDataVariant::asUInt32() const {
    return  boost::apply_visitor( uint32_t_visitor(), _internal_variant );
}

CDataVariant::operator uint32_t() const {
    return asUInt32();
}

int64_t CDataVariant::asInt64() const {
    return  boost::apply_visitor( int64_t_visitor(), _internal_variant );
}

CDataVariant::operator int64_t() const {
    return asInt64();
}

uint64_t CDataVariant::asUInt64() const {
    return  boost::apply_visitor( uint64_t_visitor(), _internal_variant );
}

CDataVariant::operator uint64_t() const {
    return asUInt64();
}

double CDataVariant::asDouble() const {
    return  boost::apply_visitor(double_visitor(), _internal_variant );
}

CDataVariant::operator double() const {
    return asDouble();
}

bool CDataVariant::asBool() const {
    return boost::apply_visitor( bool_visitor(), _internal_variant );
}

CDataVariant::operator bool() const {
    return asBool();
}

const std::string CDataVariant::asString(int precision) const {
    return  boost::apply_visitor( string_visitor(precision), _internal_variant );
}

CDataVariant::operator std::string() const {
    return asString(-1);
}

const CDataBuffer * const CDataVariant::asCDataBuffer() const {
    return boost::apply_visitor( CDataBuffer_visitor(), _internal_variant ).get();
}

CDataVariant::operator const CDataBuffer*() const {
    return asCDataBuffer();
}

CDBufferShrdPtr CDataVariant::asCDataBufferShrdPtr() {
    return boost::apply_visitor( CDataBuffer_visitor(), _internal_variant);
}

CDataVariant::operator CDBufferShrdPtr() {
    return asCDataBufferShrdPtr();
}

const CDataWrapper * const CDataVariant::asCDataWrapper() const {
    return boost::apply_visitor( CDataWrapper_visitor(), _internal_variant ).get();
}

CDataVariant::operator const CDataWrapper * const() const {
    return asCDataWrapper();
}

ChaosSharedPtr<CDataWrapper> CDataVariant::asCDataWrapperShrdPtr() {
    return boost::apply_visitor( CDataWrapper_visitor(), _internal_variant);
}

CDataVariant::operator ChaosSharedPtr<CDataWrapper>() {
    return asCDataWrapperShrdPtr();
}
