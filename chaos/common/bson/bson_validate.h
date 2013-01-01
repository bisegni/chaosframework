// bson_Validate.h

/*    Copyright 2012 10gen Inc.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#pragma once

#include <chaos/common/bson/base/status.h>
#include <chaos/common/bson/base/string_data.h>
#include <chaos/common/bson/bsontypes.h>
#include <chaos/common/bson/platform/cstdint.h>

namespace bson {

    /**
     * @param buf - bson data
     * @param maxLength - maxLength of buffer
     *                    this is NOT the bson size, but how far we know the buffer is valid
     * @param bsonLength - OUT size of bson, set if bsonLength != NULL _and_ data is valid
     */
    Status validateBSON( const char* buf, uint64_t maxLength, int* bsonLength = NULL );

}

