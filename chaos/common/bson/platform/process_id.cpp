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

#include <chaos/common/bson/platform/basic.h>

#include <chaos/common/bson/platform/process_id.h>

#include <boost/static_assert.hpp>
#include <iostream>
#include <sstream>
#include <limits>

namespace bson {

    BOOST_STATIC_ASSERT(sizeof(NativeProcessId) == sizeof(uint32_t));

    namespace {
#ifdef _WIN32
        inline NativeProcessId getCurrentNativeProcessId() { return GetCurrentProcessId(); }
#else
        inline NativeProcessId getCurrentNativeProcessId() { return getpid(); }
#endif
    }  // namespace

    ProcessId ProcessId::getCurrent() {
        return fromNative(getCurrentNativeProcessId());
    }

    int64_t ProcessId::asInt64() const {
        typedef std::numeric_limits<NativeProcessId> limits;
        if (limits::is_signed)
            return _npid;
        else
            return static_cast<int64_t>(static_cast<uint64_t>(_npid));
    }

    long long ProcessId::asLongLong() const {
        return static_cast<long long>(asInt64());
    }

    std::string ProcessId::toString() const {
        std::ostringstream os;
        os << *this;
        return os.str();
    }

    std::ostream& operator<<(std::ostream& os, ProcessId pid) {
        return os << pid.toNative();
    }

}  // namespace bson
