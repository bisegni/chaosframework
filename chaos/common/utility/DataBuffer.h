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
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/utility/ChaosAllocator.h>

#include <stdint.h>

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
                bool own;
                char *data;
                uint32_t size;
            public:
                //! init buffer will be managed by the instance of this class
                explicit ChaosBuffer(char *init_buffer,
                                     uint32_t init_buf_size,
                                     bool _own = true):
                own(_own),
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
                    CHAOS_ASSERT(own);
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
                    if ( data && own ) {
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
                explicit DataBuffer(char *init_buffer,
                                    uint32_t init_buf_size,
                                    bool _own = true):
                ChaosBuffer<Allocator>(init_buffer,
                                       init_buf_size,
                                       _own),
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
                    int32_t result = *(int32_t*)(((char*)ChaosBuffer<Allocator>::data) + cursor);
                    cursor+=sizeof(int32_t);
                    return result;
                }
                
                std::string readString(int32_t str_len) {
                    const char * str_start = ((const char *)ChaosBuffer<Allocator>::data) + cursor;
                    std::string result(str_start, str_len);
                    cursor += str_len;
                    return result;
                }
                
                std::string readStringUntilNull() {
                    const char * str_start = ((const char *)ChaosBuffer<Allocator>::data) + cursor;
                    const size_t end_string_location = std::strlen(str_start);
                    if((cursor+end_string_location) >  ChaosBuffer<Allocator>::size) return std::string();
                    std::string result(str_start, end_string_location);
                    cursor += end_string_location+1;
                    return result;
                }
                
                ChaosUniquePtr<chaos::common::data::CDataWrapper> readCDataWrapper() {
                    const char * bson_start = ((const char *)ChaosBuffer<Allocator>::data) + cursor;
                    ChaosUniquePtr<chaos::common::data::CDataWrapper> result(new chaos::common::data::CDataWrapper(bson_start));
                    cursor += result->getBSONRawSize();
                    return result;
                }
                
                ChaosSharedPtr<chaos::common::data::CDataWrapper> readCDataWrapperAsShrdPtr() {
                    const char * bson_start = ((const char *)ChaosBuffer<Allocator>::data) + cursor;
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
