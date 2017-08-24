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

#include <iosfwd>
#include <string>

#include <chaos/common/bson/base/error_codes.h>
#include <chaos/common/bson/platform/atomic_word.h>

namespace bson {

    /**
     * Status represents an error state or the absence thereof.
     *
     * A Status uses the standardized error codes -- from file 'error_codes.h' -- to
     * determine an error's cause. It further clarifies the error with a textual
     * description. Optionally, a Status may also have an error location number, which
     * should be a unique, grep-able point in the code base (including assert numbers).
     *
     * Example usage:
     *
     *    Status sumAB(int a, int b, int* c) {
     *       if (overflowIfSum(a,b)) {
     *           return Status(ErrorCodes::ERROR_OVERFLOW, "overflow in sumAB", 16494);
     *       }
     *
     *       *c = a+b;
     *       return Status::OK();
     *   }
     *
     * TODO: expand base/error_codes.h to capture common errors in current code
     * TODO: generate base/error_codes.h out of a description file
     * TODO: check 'location' duplicates against assert numbers
     */
    class Status {
    public:
        // Short-hand for returning an OK status.
        static inline Status OK();

        /**
         * Builds an error status given the error code, a textual description of what
         * caused the error, and a unique position in the where the error occurred
         * (similar to an assert number)
         */
        Status(ErrorCodes::Error code, const std::string& reason, int location = 0);
        Status(ErrorCodes::Error code, const char* reason, int location = 0);

        inline Status(const Status& other);
        inline Status& operator=(const Status& other);

#if __cplusplus >= 201103L
        inline Status(Status&& other) noexcept;
        inline Status& operator=(Status&& other) noexcept;
#endif // __cplusplus >= 201103L

        inline ~Status();

        /**
         * Returns true if 'other's error code and location are equal/different to this
         * instance's. Otherwise returns false.
         */
        bool compare(const Status& other) const;
        bool operator==(const Status& other) const;
        bool operator!=(const Status& other) const;

        /**
         * Returns true if 'other's error code is equal/different to this instance's.
         * Otherwise returns false.
         */
        bool compareCode(const ErrorCodes::Error other) const;
        bool operator==(const ErrorCodes::Error other) const;
        bool operator!=(const ErrorCodes::Error other) const;

        //
        // accessors
        //

        inline bool isOK() const;

        inline ErrorCodes::Error code() const;

        inline std::string codeString() const;

        inline std::string reason() const;

        inline int location() const;

        std::string toString() const;

        //
        // Below interface used for testing code only.
        //

        inline AtomicUInt32::WordType refCount() const;

    private:
        inline Status();

        struct ErrorInfo {
            AtomicUInt32 refs;             // reference counter
            const ErrorCodes::Error code;  // error code
            const std::string reason;      // description of error cause
            const int location;            // unique location of the triggering line in the code

            static ErrorInfo* create(ErrorCodes::Error code,
                                     const StringData& reason, int location);

            ErrorInfo(ErrorCodes::Error code, const StringData& reason, int location);
        };

        ErrorInfo* _error;

        /**
         * Increment/Decrement the reference counter inside an ErrorInfo
         *
         * @param error  ErrorInfo to be incremented
         */
        static inline void ref(ErrorInfo* error);
        static inline void unref(ErrorInfo* error);
    };

    inline bool operator==(const ErrorCodes::Error lhs, const Status& rhs);

    inline bool operator!=(const ErrorCodes::Error lhs, const Status& rhs);

    //
    // Convenience method for unittest code. Please use accessors otherwise.
    //

    std::ostream& operator<<(std::ostream& os, const Status& status);
    std::ostream& operator<<(std::ostream& os, ErrorCodes::Error);

}  // namespace bson

#include <chaos/common/bson/base/status-inl.h>
