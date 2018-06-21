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
        buffer = new char[_buffer_size];
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
    if(own_buffer) delete(buffer);
}

const char *CDataBuffer::getBuffer() const {
    return buffer;
}

uint32_t CDataBuffer::getBufferSize() const {
    return buffer_size;
}

CDataBuffer *CDataBuffer::newOwnBufferFromBuffer(char * buffer,
                                                 uint32_t _buffer_size) {
    CDataBuffer *result = new CDataBuffer();
    result->own_buffer = true;
    result->buffer = buffer;
    result->buffer_size = _buffer_size;
    return result;
}
