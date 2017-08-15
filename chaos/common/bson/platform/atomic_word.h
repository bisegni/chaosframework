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

#include <boost/static_assert.hpp>

#include <chaos/common/bson/platform/atomic_intrinsics.h>
#include <chaos/common/bson/platform/compiler.h>

namespace bson {

    /**
     * Template type for word types supporting indivisible memory operations.
     *
     * Effects of operations with "acquire" semantics are visible to other threads before effects of
     * any subsequent operation by the calling thread.  Effects of operations with "release"
     * semantics are visible to other processors only after the effects of previous operations on
     * the current thread are visible.
     *
     * NOTE(schwerin): This implementation assumes that instances are naturally aligned.
     * Instances that are not naturally aligned may operate incorrectly, or not at all.  Natural
     * alignment for this purpose means that the byte address of the beginning of the object is an
     * integer multiple of the size of the type, in bytes.
     *
     * NOTE(schwerin): This is a low level concurrency type, it is very hard to use correctly, and
     * you should not decide to use it lightly.
     *
     * NOTE(schwerin): This type is and must be a POD type, or per C++11 rules, a "Standard-layout"
     * type.
     */
    template <typename _WordType>
    class AtomicWord {
    public:
        /**
         * Underlying value type.
         */
        typedef _WordType WordType;

        /**
         * Construct a new word with the given initial value.
         */
        explicit AtomicWord(WordType value=WordType(0)) : _value(value) {}

        /**
         * Gets the current value of this AtomicWord.
         *
         * Has acquire and release semantics.
         */
        WordType load() const { return AtomicIntrinsics<WordType>::load(&_value); }

        /**
         * Gets the current value of this AtomicWord.
         *
         * Has relaxed semantics.
         */
        WordType loadRelaxed() const { return AtomicIntrinsics<WordType>::loadRelaxed(&_value); }

        /**
         * Sets the value of this AtomicWord to "newValue".
         *
         * Has acquire and release semantics.
         */
        void store(WordType newValue) { AtomicIntrinsics<WordType>::store(&_value, newValue); }

        /**
         * Atomically swaps the current value of this with "newValue".
         *
         * Returns the old value.
         *
         * Has acquire and release semantics.
         */
        WordType swap(WordType newValue) {
            return AtomicIntrinsics<WordType>::swap(&_value, newValue);
        }

        /**
         * Atomic compare and swap.
         *
         * If this value equals "expected", sets this to "newValue".
         * Always returns the original of this.
         *
         * Has acquire and release semantics.
         */
        WordType compareAndSwap(WordType expected, WordType newValue) {
            return AtomicIntrinsics<WordType>::compareAndSwap(&_value, expected, newValue);
        }

        /**
         * Get the current value of this, add "increment" and store it, atomically.
         *
         * Returns the value of this before incrementing.
         *
         * Has acquire and release semantics.
         */
        WordType fetchAndAdd(WordType increment) {
            return AtomicIntrinsics<WordType>::fetchAndAdd(&_value, increment);
        }

        /**
         * Get the current value of this, subtract "decrement" and store it, atomically.
         *
         * Returns the value of this before decrementing.
         *
         * Has acquire and release semantics.
         */
        WordType fetchAndSubtract(WordType decrement) {
            return fetchAndAdd(WordType(0) - decrement);
        }

        /**
         * Get the current value of this, add "increment" and store it, atomically.
         *
         * Returns the value of this after incrementing.
         *
         * Has acquire and release semantics.
         */
        WordType addAndFetch(WordType increment) {
            return fetchAndAdd(increment) + increment;
        }

        /**
         * Get the current value of this, subtract "decrement" and store it, atomically.
         *
         * Returns the value of this after decrementing.
         *
         * Has acquire and release semantics.
         */
        WordType subtractAndFetch(WordType decrement) {
            return fetchAndSubtract(decrement) - decrement;
        }

    private:
        volatile WordType _value;
    };

#define _ATOMIC_WORD_DECLARE(NAME, WTYPE)                               \
    typedef class AtomicWord<WTYPE> NAME;                               \
    namespace { BOOST_STATIC_ASSERT(sizeof(NAME) == sizeof(WTYPE)); }

    _ATOMIC_WORD_DECLARE(AtomicUInt32, unsigned);
    _ATOMIC_WORD_DECLARE(AtomicUInt64, unsigned long long);
    _ATOMIC_WORD_DECLARE(AtomicInt32, int);
    _ATOMIC_WORD_DECLARE(AtomicInt64, long long);
#undef _ATOMIC_WORD_DECLARE

}  // namespace bson
