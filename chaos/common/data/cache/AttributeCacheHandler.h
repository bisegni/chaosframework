/*
 *	AttributeCacheHandler.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 28/07/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_BE7A761F_8FF4_4E62_AEB8_2D484BFB4DFF_AttributeCacheHandler_h
#define __CHAOSFramework_BE7A761F_8FF4_4E62_AEB8_2D484BFB4DFF_AttributeCacheHandler_h

#include <chaos/common/data/CDataVariant.cpp>
#include <chaos/common/utility/AbstractListener.h>

namespace chaos {
    namespace common {
        namespace data {
            namespace cache {
                
                class AttributeCacheHandler:
                public chaos::common::utility::AbstractListener {
                protected:
                    virtual int preSetValue(const uint32_t  att_index,
                                            const std::string& att_name,
                                            const CDataVariant& current_value,
                                            const CDataVariant& new_value) = 0;
                    virtual int prePostValue(const uint32_t  att_index,
                                             const std::string& att_name,
                                             const CDataVariant& new_value) = 0;
                public:
                    AttributeCacheHandler();
                    ~AttributeCacheHandler();
                };
                
            }
        }
    }
}

#endif /* __CHAOSFramework_BE7A761F_8FF4_4E62_AEB8_2D484BFB4DFF_AttributeCacheHandler_h */
