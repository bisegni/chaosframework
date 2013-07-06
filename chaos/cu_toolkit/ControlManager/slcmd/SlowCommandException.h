//
//  SlowCommandException.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 6/16/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#ifndef CHAOSFramework_SlowCommandException_h
#define CHAOSFramework_SlowCommandException_h

#include <chaos/common/exception/CException.h>

namespace chaos {
    namespace cu {
        namespace cm {
            namespace slcmd {
                
                /*
                 Exception class for Slowc command package
                 */
                class SlowCommandException : public CException{
                    
                    
                public:
                    SlowCommandException(int eCode, const char * eMessage):CException(eCode, eMessage, "ControlManager::SlowControl") {
                    }
                };
                
            }
        }
    }
    
}

#endif
