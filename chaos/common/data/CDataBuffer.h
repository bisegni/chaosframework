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

#ifndef __CHAOSFramework__CDataBuffer_h
#define __CHAOSFramework__CDataBuffer_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/data/Buffer.hpp>
#include <stdint.h>

namespace chaos {
    namespace common {
        namespace data {
            class CDataBuffer;
            
            typedef ChaosUniquePtr<chaos::common::data::CDataBuffer> CDBufferUniquePtr;
            typedef ChaosSharedPtr<chaos::common::data::CDataBuffer> CDBufferShrdPtr;
            
            class CDataBuffer {
                Buffer internal_buffer;
                CDataBuffer(char *buffer,
                            uint32_t buffer_size,
                            bool own);
            public:
                CDataBuffer();
                CDataBuffer(Buffer& buffer);
                CDataBuffer(const char *buffer,
                            uint32_t buffer_size);
                CDataBuffer(const CDataBuffer& cdata_buffer);
                
                ~CDataBuffer();
                
                const char *getBuffer() const;
                
                std::size_t getBufferSize() const;
                
                static CDBufferUniquePtr newOwnBufferFromBuffer(char * buffer,
                                                                uint32_t _buffer_size);
                static CDBufferUniquePtr newOwnBufferFromBuffer(Buffer& buffer);
            };
            
            
        }
    }
}

#endif /* __CHAOSFramework__CDataBuffer_h */
