/*
 *	ByteBuffer.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 06/03/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_DB5BF462_F0F3_495E_9F27_17F8B476F473_ByteBuffer_h
#define __CHAOSFramework_DB5BF462_F0F3_495E_9F27_17F8B476F473_ByteBuffer_h

#include <stdint.h>
#include <chaos/common/utility/ChaosAllocator.h>

namespace chaos {
    namespace common {
        namespace utility {
            
#define CHAOS_BUFFER_ALLOCATION_BLOC 64
            //! implementa a byte buffer
            /*!
             allocated memory can be release for exeternam managment
             */
            template< typename Allocator =  ChaosAllocator >
            class ChaosBuffer {
                Allocator allocator;
            protected:
                void *data;
                uint32_t size;
            public:
                //! init buffer will be managed by the instance of this class
                ChaosBuffer(void *init_buffer,
                            uint32_t init_buf_size):
                data(init_buffer),
                size(init_buf_size){}
                
                ChaosBuffer(int32_t init_size = 512):
                size(init_size){
                    if ( size > 0 ) {
                        data = (char *) allocator.malloc(size);
                        if( data == NULL ) {
                            size = 0;
                        }
                    } else {
                        data = NULL;
                    }
                }
                
                ~ChaosBuffer() { kill(); }
                
                bool grow(int32_t new_len) {
                    int a = CHAOS_BUFFER_ALLOCATION_BLOC;
                    while( a < new_len ) {
                        a = a * 2;
                    }
                    data = (char *) allocator.realloc(data, (size + a));
                    if ( data == NULL ) {
                        //error
                        size = 0;
                    } else {
                        size += a;
                    }
                    return (data != NULL);
                }
                
                virtual void kill() {
                    if ( data ) {
                        allocator.free(data);
                        data = NULL;
                    }
                }
                
                virtual void* release() {
                    void * result = data;
                    data = NULL;
                    return result;
                }
                
                uint32_t getBufferSize() {
                    return size;
                }
            };
            
#define CHECK_AND_GROW(m) if(cursor+m>=ChaosBuffer<Allocator>::size) {ChaosBuffer<Allocator>::grow(m);}
            
            //!
            template< typename Allocator =  ChaosAllocator >
            class DataBuffer:
            public ChaosBuffer<Allocator> {
                int32_t cursor;
            public:
                DataBuffer(void *init_buffer,
                           uint32_t init_buf_size):
                ChaosBuffer<Allocator>(init_buffer,
                                       init_buf_size),
                cursor(0){}
                
                DataBuffer(int32_t init_size = 512):
                ChaosBuffer<Allocator>(init_size),
                cursor(0){}
                
                virtual void kill() {
                    ChaosBuffer<Allocator>::kill();
                    cursor = 0;
                }
                
                virtual void* release() {
                    void* result = ChaosBuffer<Allocator>::release();
                    cursor = 0;
                    return result;
                }
                
                void writeByte(const char * start_buf,
                               int32_t memory_len) {
                    CHECK_AND_GROW(memory_len);
                    memcpy((void*)(((char *)ChaosBuffer<Allocator>::data) + cursor), (const void *)start_buf, memory_len);
                    cursor += memory_len;
                }
                
                void writeByte(char byte) {
                    CHECK_AND_GROW(1);
                    ((char*)ChaosBuffer<Allocator>::data)[cursor++] = byte;
                }
                
                void writeInt32(const int32_t& number) {
                    CHECK_AND_GROW(sizeof(int32_t));
                    *(int32_t*)(((char*)ChaosBuffer<Allocator>::data) + cursor) = number;
                    cursor+=sizeof(int32_t);
                }
                
                int32_t readInt32() {
                    return static_cast<int32_t>(((char*)ChaosBuffer<Allocator>::data)[cursor++]);
                }
                
                std::string readString(int32_t str_len) {
                    const char * str_start = ((const char *)ChaosBuffer<Allocator>::data) + cursor;
                    std::string result(str_start, (cursor+=str_len));
                    return result;
                }
                
                void setOffset(uint32_t offset) {
                    cursor = offset;
                }
                
                uint32_t getSize() {
                    return cursor;
                }
            };
        }
    }
}

#endif /* __CHAOSFramework_DB5BF462_F0F3_495E_9F27_17F8B476F473_ByteBuffer_h */
