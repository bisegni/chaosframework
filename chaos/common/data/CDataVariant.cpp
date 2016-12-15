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
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/exception/CException.h>
using namespace chaos;
using namespace chaos::common::data;

#define SAFE_LEXICAL_WITH_DEFAULT(x, d)\
try{return x;}catch(...){return d;}

#pragma mark boolvisitor
bool bool_visitor::operator()(bool bv) const {return bv;}
bool bool_visitor::operator()(int32_t i32v) const {return (bool)i32v;}
bool bool_visitor::operator()(uint32_t ui32v) const {return (bool)ui32v;}
bool bool_visitor::operator()(int64_t i64v) const {return (bool)i64v;}
bool bool_visitor::operator()(uint64_t ui64v) const {return (bool)ui64v;}
bool bool_visitor::operator()(double dv) const {return (bool)dv;}
bool bool_visitor::operator()(const std::string& str) const {SAFE_LEXICAL_WITH_DEFAULT(boost::lexical_cast<bool>(str),false)}
bool bool_visitor::operator()(boost::shared_ptr<CDataBuffer>& buffer) const {return (bool)buffer->getBufferSize();}

#pragma mark i32visitor
int32_t int32_t_visitor::operator()(bool bv) const {return (int32_t)bv;}
int32_t int32_t_visitor::operator()(int32_t i32v) const {return i32v;}
int32_t int32_t_visitor::operator()(uint32_t ui32v) const {return (int32_t)ui32v;}
int32_t int32_t_visitor::operator()(int64_t i64v) const {return (int32_t)i64v;}
int32_t int32_t_visitor::operator()(uint64_t ui64v) const {return (int32_t)ui64v;}
int32_t int32_t_visitor::operator()(double dv) const {return (int32_t)dv;}
int32_t int32_t_visitor::operator()(const std::string& str) const {SAFE_LEXICAL_WITH_DEFAULT(boost::lexical_cast<int32_t>(str),0)}
int32_t int32_t_visitor::operator()(boost::shared_ptr<CDataBuffer>& buffer) const {return (int32_t)buffer->getBufferSize();}

#pragma mark ui32visitor
uint32_t uint32_t_visitor::operator()(bool bv) const {return (uint32_t)bv;}
uint32_t uint32_t_visitor::operator()(int32_t i32v) const {return (uint32_t)i32v;}
uint32_t uint32_t_visitor::operator()(uint32_t ui32v) const {return ui32v;}
uint32_t uint32_t_visitor::operator()(int64_t i64v) const {return (uint32_t)i64v;}
uint32_t uint32_t_visitor::operator()(uint64_t ui64v) const {return (uint32_t)ui64v;}
uint32_t uint32_t_visitor::operator()(double dv) const {return (uint32_t)dv;}
uint32_t uint32_t_visitor::operator()(const std::string& str) const {SAFE_LEXICAL_WITH_DEFAULT(boost::lexical_cast<uint32_t>(str),0)}
uint32_t uint32_t_visitor::operator()(boost::shared_ptr<CDataBuffer>& buffer) const {return buffer->getBufferSize();}

#pragma mark i64visitor
int64_t int64_t_visitor::operator()(bool bv) const {return (int64_t)bv;}
int64_t int64_t_visitor::operator()(int32_t i32v) const {return (int64_t)i32v;}
int64_t int64_t_visitor::operator()(uint32_t ui32v) const {return (int64_t)ui32v;}
int64_t int64_t_visitor::operator()(int64_t i64v) const {return i64v;}
int64_t int64_t_visitor::operator()(uint64_t ui64v) const {return (int64_t)ui64v;}
int64_t int64_t_visitor::operator()(double dv) const {return (int64_t)dv;}
int64_t int64_t_visitor::operator()(const std::string& str) const {SAFE_LEXICAL_WITH_DEFAULT(boost::lexical_cast<int64_t>(str),0)}
int64_t int64_t_visitor::operator()(boost::shared_ptr<CDataBuffer>& buffer) const {return (int64_t)buffer->getBufferSize();}

#pragma mark ui64visitor
uint64_t uint64_t_visitor::operator()(bool bv) const {return (uint64_t)bv;}
uint64_t uint64_t_visitor::operator()(int32_t i32v) const {return (uint64_t)i32v;}
uint64_t uint64_t_visitor::operator()(uint32_t ui32v) const {return (uint64_t)ui32v;}
uint64_t uint64_t_visitor::operator()(int64_t i64v) const {return (uint64_t)i64v;}
uint64_t uint64_t_visitor::operator()(uint64_t ui64v) const {return ui64v;}
uint64_t uint64_t_visitor::operator()(double dv) const {return (uint64_t)dv;}
uint64_t uint64_t_visitor::operator()(const std::string& str) const {SAFE_LEXICAL_WITH_DEFAULT(boost::lexical_cast<uint64_t>(str),0)}
uint64_t uint64_t_visitor::operator()(boost::shared_ptr<CDataBuffer>& buffer) const {return (uint64_t)buffer->getBufferSize();}

#pragma mark doublevisitor
double double_visitor::operator()(bool bv) const {return (double)bv;}
double double_visitor::operator()(int32_t i32v) const {return (double)i32v;}
double double_visitor::operator()(uint32_t ui32v) const {return (double)ui32v;}
double double_visitor::operator()(int64_t i64v) const {return (double)i64v;}
double double_visitor::operator()(uint64_t ui64v) const {return (double)ui64v;}
double double_visitor::operator()(double dv) const {return dv;}
double double_visitor::operator()(const std::string& str) const {SAFE_LEXICAL_WITH_DEFAULT(boost::lexical_cast<double>(str), false)}
double double_visitor::operator()(boost::shared_ptr<CDataBuffer>& buffer) const {return (double)buffer->getBufferSize();}

#pragma mark stringvisitor
std::string string_visitor::operator()(bool bv) const {SAFE_LEXICAL_WITH_DEFAULT(boost::lexical_cast<std::string>(bv), "false")}
std::string string_visitor::operator()(int32_t i32v) const {SAFE_LEXICAL_WITH_DEFAULT(boost::lexical_cast<std::string>(i32v), "0")}
std::string string_visitor::operator()(uint32_t ui32v) const {SAFE_LEXICAL_WITH_DEFAULT(boost::lexical_cast<std::string>(ui32v), "0")}
std::string string_visitor::operator()(int64_t i64v) const {SAFE_LEXICAL_WITH_DEFAULT(boost::lexical_cast<std::string>(i64v), "0")}
std::string string_visitor::operator()(uint64_t ui64v) const {SAFE_LEXICAL_WITH_DEFAULT(boost::lexical_cast<std::string>(ui64v), "0")}
std::string string_visitor::operator()(double dv) const {SAFE_LEXICAL_WITH_DEFAULT(boost::lexical_cast<std::string>(dv), "0")}
std::string string_visitor::operator()(const std::string& str) const {return str;}
std::string string_visitor::operator()(boost::shared_ptr<CDataBuffer>& buffer) const {return std::string(buffer->getBuffer(), buffer->getBufferSize());}
std::string string_visitor::operator()(boost::shared_ptr<CDataWrapper>& buffer) const {return buffer->getJSONString();}


#pragma mark CDataBuffervisitor
boost::shared_ptr<CDataBuffer> CDataBuffer_visitor::operator()(bool bv) const {return boost::shared_ptr<CDataBuffer>(new CDataBuffer());}
boost::shared_ptr<CDataBuffer> CDataBuffer_visitor::operator()(int32_t i32v) const {return boost::shared_ptr<CDataBuffer>(new CDataBuffer());}
boost::shared_ptr<CDataBuffer> CDataBuffer_visitor::operator()(uint32_t ui32v) const {return boost::shared_ptr<CDataBuffer>(new CDataBuffer());}
boost::shared_ptr<CDataBuffer> CDataBuffer_visitor::operator()(int64_t i64v) const {return boost::shared_ptr<CDataBuffer>(new CDataBuffer());}
boost::shared_ptr<CDataBuffer> CDataBuffer_visitor::operator()(uint64_t ui64v) const {return boost::shared_ptr<CDataBuffer>(new CDataBuffer());}
boost::shared_ptr<CDataBuffer> CDataBuffer_visitor::operator()(double dv) const {return boost::shared_ptr<CDataBuffer>(new CDataBuffer());}
boost::shared_ptr<CDataBuffer> CDataBuffer_visitor::operator()(const std::string& str) const {return boost::shared_ptr<CDataBuffer>(new CDataBuffer(str.c_str(), (uint32_t)str.size(), true));}
boost::shared_ptr<CDataBuffer> CDataBuffer_visitor::operator()(boost::shared_ptr<CDataBuffer>& buffer) const {return buffer;}
boost::shared_ptr<CDataBuffer> CDataBuffer_visitor::operator()(boost::shared_ptr<CDataWrapper>& buffer) const {return boost::shared_ptr<CDataBuffer>(new CDataBuffer(buffer->getJSONString().c_str(), (uint32_t)buffer->getJSONString().size(), true));;}

#pragma mark CDataWrappervisitor
boost::shared_ptr<CDataWrapper> CDataWrapper_visitor::operator()(bool bv) const {throw CFatalException(-1,"invalid conversion to CDataWrapper from bool",__PRETTY_FUNCTION__);return boost::shared_ptr<CDataWrapper>(new CDataWrapper());}
boost::shared_ptr<CDataWrapper> CDataWrapper_visitor::operator()(int32_t i32v) const {throw CFatalException(-1,"invalid conversion to CDataWrapper from int32_t",__PRETTY_FUNCTION__);return boost::shared_ptr<CDataWrapper>(new CDataWrapper());}
boost::shared_ptr<CDataWrapper> CDataWrapper_visitor::operator()(uint32_t ui32v) const{throw CFatalException(-1,"invalid conversion to CDataWrapper from uint32_t",__PRETTY_FUNCTION__);return boost::shared_ptr<CDataWrapper>(new CDataWrapper());}
boost::shared_ptr<CDataWrapper> CDataWrapper_visitor::operator()(int64_t i64v) const{throw CFatalException(-1,"invalid conversion to CDataWrapper from int64_t",__PRETTY_FUNCTION__);return boost::shared_ptr<CDataWrapper>(new CDataWrapper());}
boost::shared_ptr<CDataWrapper> CDataWrapper_visitor::operator()(uint64_t ui64v) const {throw CFatalException(-1,"invalid conversion to CDataWrapper from uint64_t",__PRETTY_FUNCTION__);return boost::shared_ptr<CDataWrapper>(new CDataWrapper());}
boost::shared_ptr<CDataWrapper> CDataWrapper_visitor::operator()(double dv) const {throw CFatalException(-1,"invalid conversion to CDataWrapper from double",__PRETTY_FUNCTION__);return boost::shared_ptr<CDataWrapper>(new CDataWrapper());}
boost::shared_ptr<CDataWrapper> CDataWrapper_visitor::operator()(const std::string& str) const {return boost::shared_ptr<CDataWrapper>(new CDataWrapper(str.c_str()));}
boost::shared_ptr<CDataWrapper> CDataWrapper_visitor::operator()(boost::shared_ptr<CDataBuffer>& buffer) const {return boost::shared_ptr<CDataWrapper>(new CDataWrapper(buffer->getBuffer()));}
boost::shared_ptr<CDataWrapper> CDataWrapper_visitor::operator()(boost::shared_ptr<CDataWrapper>& buffer) const {return boost::shared_ptr<CDataWrapper>(new CDataWrapper(buffer->getBSONData()->getBufferPtr()));}


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
    CDataWrapper tmp;
    try{
        tmp.setSerializedJsonData(string_value.c_str());
        type=DataType::TYPE_CLUSTER;
    } catch(...){
        type=DataType::TYPE_STRING;
    }
}


CDataVariant::CDataVariant(CDataBuffer *buffer_value)
:
type(DataType::TYPE_BYTEARRAY),
_internal_variant(boost::shared_ptr<CDataBuffer>(buffer_value)) { }

CDataVariant::CDataVariant(CDataWrapper *buffer_value)
:
type(DataType::TYPE_CLUSTER),
_internal_variant(boost::shared_ptr<CDataWrapper>(buffer_value)) { }


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
        case DataType::TYPE_BOOLEAN:
            _internal_variant = *static_cast<const bool*>(_value_pointer);
            break;
        case DataType::TYPE_INT32:
            _internal_variant = *static_cast<const int32_t*>(_value_pointer);
            break;
        case DataType::TYPE_INT64:
            _internal_variant = *static_cast<const int64_t*>(_value_pointer);
            break;
        case DataType::TYPE_DOUBLE:
            _internal_variant = *static_cast<const double*>(_value_pointer);
            break;
            
        case DataType::TYPE_CLUSTER:{
          _internal_variant = boost::shared_ptr<CDataWrapper>(new CDataWrapper(static_cast<const char*>(_value_pointer)));
        	break;
        }
        case DataType::TYPE_STRING:
            _internal_variant = std::string(static_cast<const char*>(_value_pointer),
                                            _value_size);
            break;
        case DataType::TYPE_BYTEARRAY:
            _internal_variant = boost::shared_ptr<CDataBuffer>(new CDataBuffer(static_cast<const char*>(_value_pointer),
                                                                               _value_size,
                                                                               true));
            break;
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
uint32_t CDataVariant::asUInt32() const {
    return  boost::apply_visitor( uint32_t_visitor(), _internal_variant );
}
int64_t CDataVariant::asInt64() const {
    return  boost::apply_visitor( int64_t_visitor(), _internal_variant );
}
uint64_t CDataVariant::asUInt64() const {
    return  boost::apply_visitor( uint64_t_visitor(), _internal_variant );
}
double CDataVariant::asDouble() const {
    return  boost::apply_visitor( double_visitor(), _internal_variant );
}
bool CDataVariant::asBool() const {
    return boost::apply_visitor( bool_visitor(), _internal_variant );
}
const std::string CDataVariant::asString() const {
    return  boost::apply_visitor( string_visitor(), _internal_variant );
}
const CDataBuffer * const CDataVariant::asCDataBuffer() const {
    return boost::apply_visitor( CDataBuffer_visitor(), _internal_variant ).get();
}
boost::shared_ptr<CDataBuffer> CDataVariant::asCDataBufferShrdPtr() {
    return boost::apply_visitor( CDataBuffer_visitor(), _internal_variant);
}

const CDataWrapper * const CDataVariant::asCDataWrapper() const {
    return boost::apply_visitor( CDataWrapper_visitor(), _internal_variant ).get();
}
boost::shared_ptr<CDataWrapper> CDataVariant::asCDataWrapperShrdPtr() {
    return boost::apply_visitor( CDataWrapper_visitor(), _internal_variant);
}
