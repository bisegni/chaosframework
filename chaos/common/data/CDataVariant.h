/*
 *	CDataVariant.h
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

#ifndef __CHAOSFramework__CDataVariant_h
#define __CHAOSFramework__CDataVariant_h

#include <chaos/common/chaos_constants.h>
#include <chaos/common/data/CDataBuffer.h>

#include <boost/variant.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

#define CHAOS_VARIANT_DEFINE_VISITOR_WITH_NAME_TYPE(n,t)\
class n ## _visitor:\
public boost::static_visitor< t > {\
public:\
    t operator()(bool bv) const;\
    t operator()(int32_t i32v) const;\
    t operator()(uint32_t ui32v) const;\
    t operator()(int64_t i64v) const;\
    t operator()(uint64_t ui64v) const;\
    t operator()(double dv) const;\
    t operator()(const std::string& str) const;\
    t operator()(boost::shared_ptr<chaos::common::data::CDataBuffer> buffer) const;\
};

#define CHAOS_VARIANT_DEFINE_VISITOR_WITH_TYPE(t) CHAOS_VARIANT_DEFINE_VISITOR_WITH_NAME_TYPE(t,t)

namespace chaos {
    namespace common {
        namespace data {
            CHAOS_VARIANT_DEFINE_VISITOR_WITH_TYPE(bool);
            CHAOS_VARIANT_DEFINE_VISITOR_WITH_TYPE(int32_t);
            CHAOS_VARIANT_DEFINE_VISITOR_WITH_TYPE(uint32_t);
            CHAOS_VARIANT_DEFINE_VISITOR_WITH_TYPE(int64_t);
            CHAOS_VARIANT_DEFINE_VISITOR_WITH_TYPE(uint64_t);
            CHAOS_VARIANT_DEFINE_VISITOR_WITH_TYPE(double);
            CHAOS_VARIANT_DEFINE_VISITOR_WITH_NAME_TYPE(string,std::string);
            CHAOS_VARIANT_DEFINE_VISITOR_WITH_NAME_TYPE(CDataBuffer,boost::shared_ptr<chaos::common::data::CDataBuffer>);
            
            /*!
             * Chaos variant implementation that host all dataset CHAOS data type
             */
            class CDataVariant {
                DataType::DataType type;
                boost::variant<int32_t,
                uint32_t,
                int64_t,
                uint64_t,
                double,
                bool,
                std::string,
                boost::shared_ptr<chaos::common::data::CDataBuffer> > _internal_variant;
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
                //! take the ownership of the object
                explicit CDataVariant(CDataBuffer *buffer_value);
                CDataVariant(const CDataVariant& to_copy);
                CDataVariant();
                
                CDataVariant& operator=(const CDataVariant& arg);
                
                DataType::DataType getType() const;
                bool isValid() const;
                
                int32_t asInt32() const;
                uint32_t asUInt32() const;
                int64_t asInt64() const;
                uint64_t asUInt64() const;
                double asDouble() const;
                bool asBool() const;
                const std::string asString() const;
                const chaos::common::data::CDataBuffer *const asCDataBuffer() const;
            };
        }
    }
}

#endif /* __CHAOSFramework__CDataVariant_h */
