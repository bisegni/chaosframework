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

#ifndef __CHAOSFramework_DB5BF462_F0F3_495E_9F27_17F8B476F473_ByteBuffer_h
#define __CHAOSFramework_DB5BF462_F0F3_495E_9F27_17F8B476F473_ByteBuffer_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/data/Buffer.hpp>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/utility/ChaosAllocator.h>

#include <stdint.h>
#include <cassert>      //assert

namespace chaos {
    namespace common {
        namespace utility {
            
#define CHECK_AND_GROW(m) if(cursor+m>=ChaosBuffer<Allocator>::size) {ChaosBuffer<Allocator>::grow(m);}
            
            //!
            class DataBuffer {
                data::Buffer buffer;
                int32_t cursor;
            public:
                explicit DataBuffer(char *init_buffer,
                                    uint32_t init_buf_size,
                                    bool _own = true):
                buffer(init_buffer,
                            init_buf_size,
                            init_buf_size,
                            _own),
                cursor(0){}
                
                DataBuffer():
                buffer(),
                cursor(0){}
                
                virtual void kill() {
                    buffer.clear();
                    cursor = 0;
                }
                
                virtual void* release() {
                    void* result = buffer.detach();
                    cursor = 0;
                    return result;
                }
                
                void writeByte(const char * start_buf,
                               int32_t memory_len) {
                    buffer.append(start_buf, memory_len);
                    cursor += memory_len;
                }
                
                void writeByte(char byte) {
                    buffer.append(&byte, 1);
                }
                
                void writeInt32(const int32_t& number) {
                    buffer.append(&number, sizeof(int32_t));
                    cursor+=sizeof(int32_t);
                }
                
                int32_t readInt32() {
                    int32_t result = *(int32_t*)(buffer.data() + cursor);
                    cursor+=sizeof(int32_t);
                    return result;
                }
                
                std::string readString(int32_t str_len) {
                    const char * str_start = (buffer.data() + cursor);
                    std::string result(str_start, str_len);
                    cursor += str_len;
                    return result;
                }
                
                std::string readStringUntilNull() {
                    const char * str_start = (buffer.data() + cursor);
                    const size_t end_string_location = std::strlen(str_start);
                    if((cursor+end_string_location) >  buffer.size()) return std::string();
                    std::string result(str_start, end_string_location);
                    cursor += end_string_location+1;
                    return result;
                }
                
                ChaosUniquePtr<chaos::common::data::CDataWrapper> readCDataWrapper() {
                    const char * bson_start = (buffer.data() + cursor);
                    ChaosUniquePtr<chaos::common::data::CDataWrapper> result(new chaos::common::data::CDataWrapper(bson_start));
                    cursor += result->getBSONRawSize();
                    return result;
                }
                
                ChaosSharedPtr<chaos::common::data::CDataWrapper> readCDataWrapperAsShrdPtr() {
                    const char * bson_start = (buffer.data() + cursor);
                    ChaosSharedPtr<chaos::common::data::CDataWrapper> result(new chaos::common::data::CDataWrapper(bson_start));
                    cursor += result->getBSONRawSize();
                    return result;
                }
                
                void setOffset(uint32_t offset) {
                    cursor = offset;
                }
                
                uint32_t getCursorLocation() {
                    return cursor;
                }
            };
        }
    }
}

#endif /* __CHAOSFramework_DB5BF462_F0F3_495E_9F27_17F8B476F473_ByteBuffer_h */
