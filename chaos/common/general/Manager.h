    //
    //  CManager.h
    //  ChaosFramework
    //
    //  Created by Claudio Bisegni on 13/03/11.
    //  Copyright 2011 INFN. All rights reserved.
    //

#ifndef CManager_H
#define CManager_H

#include <chaos/common/exception/CException.h>
namespace chaos{
    
    class Manager {
    public:
        /*
         * Initzialize the dataCManager
         */
        virtual void init() throw(CException) = 0;
        
        /*
         * Deinitzialize the dataCManager
         */
        virtual void deinit() throw(CException) = 0;
        
        /*
         * Start all sub process
         */
        virtual void start() throw(CException) = 0;
    };
}
#endif
