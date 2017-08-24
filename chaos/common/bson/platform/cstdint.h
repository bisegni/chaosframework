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

/**
 * Include "mongo/platform/cstdint.h" to get the C++11 cstdint types in namespace bson.
 */

#if defined(_MSC_VER)
#include <cstdint>
#define _BSON_MONGO_STDINT_NAMESPACE std
#elif defined(__GNUC__)
#include <stdint.h>
#define _BSON_MONGO_STDINT_NAMESPACE
#else
#error "Unsupported compiler family"
#endif

namespace bson {
    using _BSON_MONGO_STDINT_NAMESPACE::int8_t;
    using _BSON_MONGO_STDINT_NAMESPACE::int16_t;
    using _BSON_MONGO_STDINT_NAMESPACE::int32_t;
    using _BSON_MONGO_STDINT_NAMESPACE::int64_t;
    using _BSON_MONGO_STDINT_NAMESPACE::intptr_t;

    using _BSON_MONGO_STDINT_NAMESPACE::uint8_t;
    using _BSON_MONGO_STDINT_NAMESPACE::uint16_t;
    using _BSON_MONGO_STDINT_NAMESPACE::uint32_t;
    using _BSON_MONGO_STDINT_NAMESPACE::uint64_t;
    using _BSON_MONGO_STDINT_NAMESPACE::uintptr_t;
}  // namespace bson

#undef _BSON_MONGO_STDINT_NAMESPACE
