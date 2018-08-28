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
#include "Base64Util.h"
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/ostream_iterator.hpp>
#include <sstream>
#include <string>


using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace boost::archive::iterators;

typedef base64_from_binary<transform_width<const char *,6,8> > base64_enc;
typedef transform_width<binary_from_base64<const char *>, 8, 6> base64_dec;
typedef insert_linebreaks<base64_enc, 72> base64_enc_formatted;

std::string Base64Util::encode(const char * buffer, size_t len) {
    std::stringstream os;
    std::copy(base64_enc(buffer),
              base64_enc(buffer + len),
              ostream_iterator<char>(os));
    
    return os.str();
}

std::string Base64Util::encode(chaos::common::data::CDataBuffer& buffer) {
    return encode(buffer.getBuffer(), buffer.getBufferSize());
}

std::string Base64Util::encode_formatted(const char * buffer, size_t len) {
    std::stringstream os;
    std::copy(base64_enc_formatted(buffer),
              base64_enc_formatted(buffer + len),
              ostream_iterator<char>(os));
    return os.str();
}

std::string Base64Util::encode_formatted(chaos::common::data::CDataBuffer& buffer) {
    return encode_formatted(buffer.getBuffer(), buffer.getBufferSize());
}

class buffer_inserter :
public std::iterator<std::output_iterator_tag, char>{
    Buffer &b;
public:
    buffer_inserter(Buffer &_b) : b(_b) { }
    buffer_inserter operator ++ (int) { return *this; }
    buffer_inserter operator ++ () { return *this; }
    buffer_inserter operator * () { return *this; }
    void operator = (const char &val) {
        b.append(&val, 1);
    }
};

CDBufferUniquePtr Base64Util::decode(const std::string& b64_string) {
    size_t size = b64_string.size();
    // Remove the padding characters, cf. https://svn.boost.org/trac/boost/ticket/5629
    if (size && b64_string[size - 1] == '=') {
        --size;
        if (size && b64_string[size - 1] == '=') --size;
    }
    if (size == 0) return CDBufferUniquePtr();
    Buffer buf;
    std::copy(base64_dec(b64_string.data()),
              base64_dec(b64_string.data() + size),
              buffer_inserter(buf));
    return CDataBuffer::newOwnBufferFromBuffer(buf);
}
