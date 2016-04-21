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

namespace chaos {
    namespace common {
        namespace data {

          /*!
           * Chaos variant implementation that host all dataset CHAOS data type
           */
            class CDataVariant {
                DataType::DataType type;
              boost::variant<int32_t,
                             int64_t,
                             double,
                             bool,
                             std::string,
                             boost::shared_ptr<CDataBuffer> > _internal_variant;
            public:
                explicit CDataVariant(int32_t int32_value);
                explicit CDataVariant(int64_t int64_value);
                explicit CDataVariant(double double_value);
                explicit CDataVariant(bool boolvalue);
                explicit CDataVariant(const std::string& string_value);
              //! take the ownership of the object
              explicit CDataVariant(CDataBuffer *buffer_value);
                CDataVariant(const CDataVariant& to_copy);
                CDataVariant();
                DataType::DataType getType() const;
                
                int32_t asInt32() const;
                int64_t asInt64() const;
                double asDouble() const;
                bool asBool() const;
                const std::string& asString() const;
              const CDataBuffer *const asCDataBuffer() const;
            };
        }
    }
}

#endif /* __CHAOSFramework__CDataVariant_h */
