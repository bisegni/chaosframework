/*	
 *	Atomic.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *	
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
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
        
#if defined(__GNUC__) && ((__GNUC__*10 + __GNUC_MINOR__) < 41)
        
        typedef int atomic_int_type;
        
        static inline
        atomic_int_type atomic_increment(volatile atomic_int_type* p)
        {
            return  __gnu_cxx::__exchange_and_add(p, 1);
        }
        
#elif defined(__GNUC__) && ((__GNUC__*10 + __GNUC_MINOR__) >= 41)
        
        typedef int atomic_int_type;
        
        static inline
        atomic_int_type atomic_increment(volatile atomic_int_type* p)
        {
            return __sync_fetch_and_add(p, 1);
        }
        
#else
        
        typedef boost::uint32_t atomic_int_type;
        
        static inline
        atomic_int_type atomic_increment(volatile atomic_int_type* p)
        {
            return boost::interprocess::detail::atomic_inc32(p);
        }
        
#endif
}
#endif
