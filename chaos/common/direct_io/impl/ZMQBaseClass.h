//
//  ZMQBaseClass.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 08/02/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#ifndef CHAOSFramework_ZMQGlobalUtils_h
#define CHAOSFramework_ZMQGlobalUtils_h

namespace chaos {
	namespace common {
		namespace direct_io {
            namespace impl {
                
                class ZMQBaseClass {
                protected:
                    int closeSocketNoWhait (void *socket);
                };
                
            }
        }
    }
}



#endif
