//
//  micro_unit_toolkit_types.h
//  CHAOSFramework
//
//  Created by bisegni on 02/08/2017.
//  Copyright © 2017 INFN. All rights reserved.
//

#ifndef micro_unit_toolkit_types_h
#define micro_unit_toolkit_types_h
#if __cplusplus < 201103L || FORCE_BOOST_SHPOINTER
    #include <boost/shared_ptr.hpp>
#endif
namespace chaos {
    namespace micro_unit_toolkit {
#if __cplusplus >= 201103L
template<typename T>
using ChaosUniquePtr = std::unique_ptr<T>;
  #ifndef FORCE_BOOST_SHPOINTER
    template<typename T>
    using ChaosSharedPtr = std::shared_ptr<T>;
  #else

    #define ChaosSharedPtr boost::shared_ptr
  #endif
#define ChaosMoveOperator(x) std::move(x)
#else
#define ChaosSharedPtr boost::shared_ptr
#define ChaosUniquePtr std::auto_ptr
#define ChaosMoveOperator(x) x
#endif
        static unsigned int CommunicationTimeout = 5000;
    }
}

#endif /* micro_unit_toolkit_types_h */
