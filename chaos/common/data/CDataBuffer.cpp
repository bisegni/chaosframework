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
    internal_buffer(){}

CDataBuffer::CDataBuffer(const char *_buffer,
                         uint32_t _buffer_size):
internal_buffer(_buffer,
                _buffer_size){}

CDataBuffer::CDataBuffer(char *buffer,
                         uint32_t buffer_size,
                         bool own):
internal_buffer(buffer,
                buffer_size,
                buffer_size,
                true){}

CDataBuffer::CDataBuffer(const CDataBuffer& cdata_buffer):
internal_buffer(cdata_buffer.internal_buffer){}

CDataBuffer::~CDataBuffer() {}

const char *CDataBuffer::getBuffer() const {
    return internal_buffer.data();
}

std::size_t CDataBuffer::getBufferSize() const {
    return internal_buffer.size();
}

CDBufferUniquePtr CDataBuffer::newOwnBufferFromBuffer(char * buffer,
                                                      uint32_t _buffer_size) {
    return CDBufferUniquePtr(new CDataBuffer(buffer, _buffer_size, true));
}

CDBufferUniquePtr CDataBuffer::newOwnBufferFromBuffer(Buffer& buffer) {
    size_t size = buffer.size();
    char * buff = buffer.detach();
    return CDBufferUniquePtr(new CDataBuffer(buff, (uint32_t)size, true));
}

CDataBuffer& CDataBuffer::operator=(CDataBuffer const &rhs) {
    if (this != &rhs) {
        internal_buffer = rhs.internal_buffer;
    }
    return *this;
};
