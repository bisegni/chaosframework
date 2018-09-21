/*
 * Copyright 2012, 25/07/2018 INFN
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

#ifndef __CHAOSFramework__Base64Util_h
#define __CHAOSFramework__Base64Util_h

#include <string>
#include <chaos/common/data/CDataBuffer.h>

namespace chaos {
    namespace common {
        namespace utility {
            class Base64Util {
            public:
                static std::string encode(const char * buffer, size_t len);
                static std::string encode(chaos::common::data::CDataBuffer& buffer);
                
                static std::string encode_formatted(const char * buffer, size_t len);
                static std::string encode_formatted(chaos::common::data::CDataBuffer& buffer);
                
                static chaos::common::data::CDBufferUniquePtr decode(const std::string& b64_string);
            };
        }
    }
}

#endif /* __CHAOSFramework__Base64Util_h */
