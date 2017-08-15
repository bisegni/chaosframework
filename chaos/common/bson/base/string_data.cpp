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

#include <chaos/common/bson/base/string_data.h>

#include <ostream>
#include <chaos/common/bson/murmurhash3/MurmurHash3.h>

namespace bson {

    namespace {

        template <int SizeofSizeT>
        size_t murmur3(const StringData& str);

        template <>
        size_t murmur3<4>(const StringData& str) {
            uint32_t hash;
            MurmurHash3_x86_32(str.rawData(), str.size(), 0, &hash);
            return hash;
        }

        template <>
        size_t murmur3<8>(const StringData& str) {
            uint64_t hash[2];
            MurmurHash3_x64_128(str.rawData(), str.size(), 0, hash);
            return static_cast<size_t>(hash[0]);
        }

    }  // namespace

    std::ostream& operator<<(std::ostream& stream, const StringData& value) {
        return stream.write(value.rawData(), value.size());
    }

    size_t StringData::Hasher::operator() (const StringData& str) const {
        return murmur3<sizeof(size_t)>(str);
    }

} // namespace bson
