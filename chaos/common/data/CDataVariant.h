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

#ifndef __CHAOSFramework__CDataVariant_h
#define __CHAOSFramework__CDataVariant_h

#include <chaos/common/chaos_constants.h>
//#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/data/CDataBuffer.h>

#include <boost/variant.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

#define CHAOS_VARIANT_DEFINE_VISITOR_WITH_NAME_TYPE(n,t)\
class n ## _visitor:\
public boost::static_visitor< t > {\
public:\
t operator()(const bool bv) const;\
t operator()(const int32_t i32v) const;\
t operator()(const uint32_t ui32v) const;\
t operator()(const int64_t i64v) const;\
t operator()(const uint64_t ui64v) const;\
t operator()(double dv) const;\
t operator()(const std::string& str) const;\
t operator()(const ChaosSharedPtr<chaos::common::data::CDataBuffer>& buffer) const;\
t operator()(const ChaosSharedPtr<chaos::common::data::CDataWrapper>& buffer) const;\
};

#define CHAOS_VARIANT_DEFINE_VISITOR_WITH_NAME_TYPE_PARAM(n,t,p)\
class n ## _visitor:\
public boost::static_visitor< t > {\
p param;\
public:\
n ## _visitor(p _param):param(_param){};\
t operator()(const bool bv) const;\
t operator()(const int32_t i32v) const;\
t operator()(const uint32_t ui32v) const;\
t operator()(const int64_t i64v) const;\
t operator()(const uint64_t ui64v) const;\
t operator()(double dv) const;\
t operator()(const std::string& str) const;\
t operator()(const ChaosSharedPtr<chaos::common::data::CDataBuffer>& buffer) const;\
t operator()(const ChaosSharedPtr<chaos::common::data::CDataWrapper>& buffer) const;\
};

#define CHAOS_VARIANT_DEFINE_VISITOR_WITH_TYPE(t) CHAOS_VARIANT_DEFINE_VISITOR_WITH_NAME_TYPE(t,t)

namespace chaos {
    namespace common {
        namespace data {
            class CDataWrapper;
            CHAOS_VARIANT_DEFINE_VISITOR_WITH_TYPE(bool);
            CHAOS_VARIANT_DEFINE_VISITOR_WITH_TYPE(int32_t);
            CHAOS_VARIANT_DEFINE_VISITOR_WITH_TYPE(uint32_t);
            CHAOS_VARIANT_DEFINE_VISITOR_WITH_TYPE(int64_t);
            CHAOS_VARIANT_DEFINE_VISITOR_WITH_TYPE(uint64_t);
            CHAOS_VARIANT_DEFINE_VISITOR_WITH_TYPE(double);
            CHAOS_VARIANT_DEFINE_VISITOR_WITH_NAME_TYPE_PARAM(string, std::string, int);
            CHAOS_VARIANT_DEFINE_VISITOR_WITH_NAME_TYPE(CDataBuffer, ChaosSharedPtr<chaos::common::data::CDataBuffer>);
            CHAOS_VARIANT_DEFINE_VISITOR_WITH_NAME_TYPE(CDataWrapper, ChaosSharedPtr<chaos::common::data::CDataWrapper>);
            
            
            /*!
             * Chaos variant implementation that host all dataset CHAOS data type
             */
            class CDataVariant {
                friend bool operator==(const CDataVariant& lhs, const CDataVariant& rhs);
                DataType::DataType type;
                boost::variant<int32_t,
                uint32_t,
                int64_t,
                uint64_t,
                double,
                bool,
                std::string,
                ChaosSharedPtr<chaos::common::data::CDataBuffer>,
                ChaosSharedPtr<chaos::common::data::CDataWrapper> > _internal_variant;
                
                inline static void dtoS(double source,
                                        std::string& converted,
                                        unsigned int precision);
            public:
                explicit CDataVariant(DataType::DataType _type,
                                      const void *_value_pointer,
                                      uint32_t _value_size);
                explicit CDataVariant(int32_t int32_value);
                explicit CDataVariant(uint32_t int32_value);
                explicit CDataVariant(int64_t int64_value);
                explicit CDataVariant(uint64_t int64_value);
                explicit CDataVariant(double double_value);
                explicit CDataVariant(bool boolvalue);
                explicit CDataVariant(const std::string& string_value);
                explicit CDataVariant(const char * string_value);
                //! take the ownership of the object
                explicit CDataVariant(CDBufferUniquePtr buffer_value);
                explicit CDataVariant(CDataWrapper *buffer_value);
                
                CDataVariant(const CDataVariant& to_copy);
                
                CDataVariant();
                
                CDataVariant& operator=(const CDataVariant& arg);
                
                DataType::DataType getType() const;
                bool isValid() const;
                
                char asChar() const;
                operator char() const;
                
                int32_t asInt32() const;
                operator int32_t() const;
                
                uint32_t asUInt32() const;
                operator uint32_t() const;
                
                int64_t asInt64() const;
                operator int64_t() const;
                
                uint64_t asUInt64() const;
                operator uint64_t() const;
                operator long long int() const;

                double asDouble() const;
                operator double() const;
                
                bool asBool() const;
                operator bool() const;
                
                const std::string asString(int precision = -1) const;
                operator std::string() const;
                
                const chaos::common::data::CDataBuffer *const asCDataBuffer() const;
                operator const chaos::common::data::CDataBuffer *() const;
                
                CDBufferShrdPtr asCDataBufferShrdPtr();
                operator CDBufferShrdPtr();
                
                const chaos::common::data::CDataWrapper *const asCDataWrapper() const;
                operator const chaos::common::data::CDataWrapper *const() const;
                
                ChaosSharedPtr<CDataWrapper> asCDataWrapperShrdPtr();
                operator ChaosSharedPtr<CDataWrapper>();
            };
        }
    }
}

#endif /* __CHAOSFramework__CDataVariant_h */
