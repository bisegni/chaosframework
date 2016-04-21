/*
 *	CDataBuffer.h
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

#ifndef __CHAOSFramework__CDataBuffer_h
#define __CHAOSFramework__CDataBuffer_h

#include <stdint.h>

namespace chaos {
    namespace common {
        namespace data {
            
            class CDataBuffer {
                bool own_buffer;
                char * buffer;
                uint32_t buffer_size;
                
            public:
              CDataBuffer();
                CDataBuffer(const char *buffer,
                            uint32_t buffer_size,
                            bool copy = false);
                CDataBuffer(const CDataBuffer& cdata_buffer);
                
                ~CDataBuffer();
                
                const char *getBuffer() const;
                
                uint32_t getBufferSize() const;
            };
            
        }
    }
}

#endif /* __CHAOSFramework__CDataBuffer_h */
