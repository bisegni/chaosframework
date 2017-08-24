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

/**
 * Utility functions for parsing numbers from strings.
 */

#pragma once

#include <chaos/common/bson/base/status.h>
#include <chaos/common/bson/base/string_data.h>

namespace bson {

    /**
     * Parses a number out of a StringData.
     *
     * Parses "stringValue", interpreting it as a number of the given "base".  On success, stores
     * the parsed value into "*result" and returns Status::OK().
     *
     * Valid values for "base" are 2-36, with 0 meaning "choose the base by inspecting the prefix
     * on the number", as in strtol.  Returns Status::BadValue if an illegal value is supplied for
     * "base".
     *
     * The entirety of the string must consist of digits in the given base, except optionally the
     * first character may be "+" or "-", and hexadecimal numbers may begin "0x".  Same as strtol,
     * without the property of stripping whitespace at the beginning, and fails to parse if there
     * are non-digit characters at the end of the string.
     *
     * See parse_number.cpp for the available instantiations, and add any new instantiations there.
     */
    template <typename NumberType>
    Status parseNumberFromStringWithBase(const StringData& stringValue, int base, NumberType* result);

    template <typename NumberType>
    static Status parseNumberFromString(const StringData& stringValue, NumberType* result) {
        return parseNumberFromStringWithBase(stringValue, 0, result);
    }

}  // namespace bson
