/*
 *	CDataBuffer.cpp
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

#include <chaos/common/data/CDataBuffer.h>

#include <cstring>
#include <stdlib.h>

using namespace chaos::common::data;
CDataBuffer::CDataBuffer():
    own_buffer(false),
    buffer_size(0),
    buffer(NULL) { }

CDataBuffer::CDataBuffer(const char *_buffer,
                         uint32_t _buffer_size,
                         bool copy):
own_buffer(copy) {
    buffer_size = _buffer_size;
    if(copy) {
        buffer = (char*)malloc(_buffer_size);
        memcpy(buffer, _buffer, buffer_size);
    } else {
        buffer = (char*)_buffer;
    }
}

CDataBuffer::CDataBuffer(const CDataBuffer& cdata_buffer):
own_buffer(cdata_buffer.own_buffer),
buffer(cdata_buffer.buffer),
buffer_size(cdata_buffer.buffer_size){}

CDataBuffer::~CDataBuffer() {
    if(own_buffer) free(buffer);
}

const char *CDataBuffer::getBuffer() const {
    return buffer;
}

uint32_t CDataBuffer::getBufferSize() const {
    return buffer_size;
}


