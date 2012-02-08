//
//  Atomic.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 23/01/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

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
