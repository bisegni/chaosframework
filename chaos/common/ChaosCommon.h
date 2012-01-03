    //
    //  Common.h
    //  ControlSystemLib
    //
    //  Created by Bisegni Claudio on 19/12/11.
    //  Copyright (c) 2011 INFN. All rights reserved.
    //

#ifndef ControlSystemLib_ChaosCommon_h
#define ControlSystemLib_ChaosCommon_h

#include <chaos/common/configuration/GlobalConfiguration.h>

namespace chaos {
    
    /*
     Chaos base libreary for other toolkit sublcass
     */
    class ChaosCommon {
        
    protected:
        ChaosCommon();
        ~ChaosCommon();

        /*
         !CHAOS substrate initialization
         */
        virtual void init(int argc = 1, const char* argv[] = NULL) throw(CException);
        /*
         Try to search an ip for the local network interface
         */
        void scanForLocalNetworkAddress();

    public:
        
        /*
         return the global configuration
         */
        GlobalConfiguration *getGlobalConfigurationInstance();
    }; 
}


#endif
