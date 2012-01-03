//
//  CConfigurable.h
//  ControlSystemLib
//
//  Created by bisegni on 23/06/11.
//  Copyright 2011 INFN. All rights reserved.
//

#ifndef ChaosLib_CConfigurable_h
#define ChaosLib_CConfigurable_h

#include <boost/shared_ptr.hpp>

#include <chaos/common/data/CDataWrapper.h>

namespace chaos {
    
    /**
     Define a rpotocol for update configuration in a class
     */
    class Configurable {
        
    public:
        virtual CDataWrapper* updateConfiguration(CDataWrapper*) = 0;
    };
}

#endif
