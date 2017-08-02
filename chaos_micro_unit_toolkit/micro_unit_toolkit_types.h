//
//  micro_unit_toolkit_types.h
//  CHAOSFramework
//
//  Created by bisegni on 02/08/2017.
//  Copyright © 2017 INFN. All rights reserved.
//

#ifndef micro_unit_toolkit_types_h
#define micro_unit_toolkit_types_h

#include <memory>

namespace chaos {
    namespace micro_unit_toolkit {
#if __cplusplus >= 201103L
        template<typename T>
        using ChaosUniquePtr = std::unique_ptr<T>;
#define ChaosMoveOperator(x) std::move(x)
#else
        template<typename T>
        using ChaosUniquePtr = std::auto_ptr<T>;
#define ChaosMoveOperator(x) x

#endif
    }
}



#endif /* micro_unit_toolkit_types_h */
