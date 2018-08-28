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
            
            //!type def for shamrt pointer
            typedef ChaosUniquePtr<chaos::common::data::CDataBuffer> CDBufferUniquePtr;
            //!type def for shamrt pointer
            typedef ChaosSharedPtr<chaos::common::data::CDataBuffer> CDBufferShrdPtr;
            
            class CDataBuffer {
                //!internal buffer engine
                Buffer internal_buffer;
                /**
                 * @brief Construct a new CDataBuffer object
                 * 
                 * @param buffer 
                 * @param buffer_size 
                 * @param own 
                 */
                CDataBuffer(char *buffer,
                            uint32_t buffer_size,
                            bool own);
            public:
                /**
                 * @brief Construct a new CDataBuffer object
                 * 
                 */
                CDataBuffer();

                /**
                 * @brief Construct a new CDataBuffer object
                 * 
                 * @param buffer 
                 */
                explicit CDataBuffer(Buffer& buffer);

                /**
                 * @brief Construct a new CDataBuffer object
                 * 
                 * @param buffer 
                 * @param buffer_size 
                 */
                CDataBuffer(const char *buffer,
                            uint32_t buffer_size);
                
                /**
                 * @brief Construct a new CDataBuffer object
                 * 
                 * @param cdata_buffer 
                 */
                explicit CDataBuffer(const CDataBuffer& cdata_buffer);
                
                /**
                 * @brief Destroy the CDataBuffer object
                 * 
                 */
                ~CDataBuffer();
                /**
                 * @brief Get the Buffer object
                 * 
                 * @return const char* 
                 */
                const char *getBuffer() const;
                
                /**
                 * @brief Get the Buffer Size object
                 * 
                 * @return std::size_t 
                 */
                std::size_t getBufferSize() const;
                
                /**
                 * @brief 
                 * 
                 * @param buffer 
                 * @param _buffer_size 
                 * @return CDBufferUniquePtr 
                 */
                static CDBufferUniquePtr newOwnBufferFromBuffer(char * buffer,
                                                                uint32_t _buffer_size);

                /**
                 * @brief 
                 * 
                 * @param buffer 
                 * @return CDBufferUniquePtr 
                 */
                static CDBufferUniquePtr newOwnBufferFromBuffer(Buffer& buffer);

                /**
                 * @brief 
                 * 
                 * @param rhs 
                 * @return CDataBuffer& 
                 */
                CDataBuffer& operator=(CDataBuffer const &rhs);
            };
        }
    }
}

#endif /* __CHAOSFramework__CDataBuffer_h */
