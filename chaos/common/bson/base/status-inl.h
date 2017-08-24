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

namespace bson {

    inline Status Status::OK() {
        return Status();
    }

    inline Status::Status(const Status& other)
        : _error(other._error) {
        ref(_error);
    }

    inline Status& Status::operator=(const Status& other) {
        ref(other._error);
        unref(_error);
        _error = other._error;
        return *this;
    }

#if __cplusplus >= 201103L
    inline Status::Status(Status&& other) noexcept
        : _error(other._error) {
        other._error = nullptr;
    }

    inline Status& Status::operator=(Status&& other) noexcept {
        unref(_error);
        _error = other._error;
        other._error = nullptr;
        return *this;
    }
#endif // __cplusplus >= 201103L

    inline Status::~Status() {
        unref(_error);
    }

    inline bool Status::isOK() const {
        return code() == ErrorCodes::OK;
    }

    inline ErrorCodes::Error Status::code() const {
        return _error ? _error->code : ErrorCodes::OK;
    }

    inline std::string Status::codeString() const {
        return ErrorCodes::errorString(code());
    }

    inline std::string Status::reason() const {
        return _error ? _error->reason : std::string();
    }

    inline int Status::location() const {
        return _error ? _error->location : 0;
    }

    inline AtomicUInt32::WordType Status::refCount() const {
        return _error ? _error->refs.load() : 0;
    }

    inline Status::Status()
        : _error(NULL) {
    }

    inline void Status::ref(ErrorInfo* error) {
        if (error)
            error->refs.fetchAndAdd(1);
    }

    inline void Status::unref(ErrorInfo* error) {
        if (error && (error->refs.subtractAndFetch(1) == 0))
            delete error;
    }

    inline bool operator==(const ErrorCodes::Error lhs, const Status& rhs) {
        return rhs == lhs;
    }

    inline bool operator!=(const ErrorCodes::Error lhs, const Status& rhs) {
        return rhs != lhs;
    }

} // namespace bson
