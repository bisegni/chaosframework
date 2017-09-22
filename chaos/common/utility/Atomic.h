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
#ifndef ChaosFramework_Atomic_h
#define ChaosFramework_Atomic_h

#if defined(__GNUC__) && ((__GNUC__*10 + __GNUC_MINOR__) < 41)
#  include <bits/atomicity.h>
#elif defined(__GNUC__) && ((__GNUC__*10 + __GNUC_MINOR__) >= 41)
# // use GCC builtins
#else
#  include <boost/cstdint.hpp>
#  include <boost/interprocess/detail/atomic.hpp>
#endif

namespace chaos {
	namespace common {
		namespace utility {
#if defined(__GNUC__) && ((__GNUC__*10 + __GNUC_MINOR__) < 41)
			
			typedef unsigned int atomic_int_type;
			
			static inline
			atomic_int_type atomic_increment(volatile atomic_int_type* p)
			{
				return  __gnu_cxx::__exchange_and_add(p, 1);
			}
			
			static inline
			atomic_int_type atomic_decrement(volatile atomic_int_type* p)
			{
				return  __gnu_cxx::__exchange_and_add(p, -1);
			}
			
#elif defined(__GNUC__) && ((__GNUC__*10 + __GNUC_MINOR__) >= 41)
			
			typedef unsigned int atomic_int_type;
			
			static inline
			atomic_int_type atomic_increment(volatile atomic_int_type* p) {
				return __sync_fetch_and_add(p, 1);
			}
			
			static inline
			atomic_int_type atomic_decrement(volatile atomic_int_type* p) {
				return __sync_fetch_and_add(p, -1);
			}
#else
			
			typedef boost::uint32_t atomic_int_type;
			
			static inline
			atomic_int_type atomic_increment(volatile atomic_int_type* p)
			{
				return boost::interprocess::detail::atomic_inc32(p);
			}
			
			static inline
			atomic_int_type atomic_decrement(volatile atomic_int_type* p)
			{
				return boost::interprocess::detail::atomic_decrement(p);
			}
#endif
		}
	}
}
#endif
