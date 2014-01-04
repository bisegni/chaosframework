//
//  BatchCommandException.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 6/16/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#ifndef CHAOSFramework_BatchCommandException_h
#define CHAOSFramework_BatchCommandException_h

#include <chaos/common/exception/CException.h>

namespace chaos {
    namespace common {
        namespace batch_command {
            
            /*
             Exception class for Slowc command package
             */
            class BatchCommandException : public CException{
                
                
            public:
                BatchCommandException(int eCode, const char * eMessage):CException(eCode, eMessage, "ControlManager::SlowControl") {}
            };
            
        }
    }
}

#endif
