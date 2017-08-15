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

#pragma once

#include <vector>

#include <chaos/common/bson/platform/cstdint.h>

namespace bson {
namespace mutablebson {

    // A damage event represents a change of size 'size' byte at starting at offset
    // 'target_offset' in some target buffer, with the replacement data being 'size' bytes of
    // data from the 'source' offset. The base addresses against which these offsets are to be
    // applied are not captured here.
    struct DamageEvent {
        typedef uint32_t OffsetSizeType;

        // Offset of source data (in some buffer held elsewhere).
        OffsetSizeType sourceOffset;

        // Offset of target data (in some buffer held elsewhere).
        OffsetSizeType targetOffset;

        // Size of the damage region.
        size_t size;
    };

    typedef std::vector<DamageEvent> DamageVector;

} // namespace mutablebson
} // namespace bson
