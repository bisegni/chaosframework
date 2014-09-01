//
//  ZMQBaseClass.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 08/02/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#ifndef CHAOSFramework_ZMQGlobalUtils_h
#define CHAOSFramework_ZMQGlobalUtils_h

#include <string>
namespace chaos {
	namespace common {
		namespace direct_io {
            namespace impl {
                
				const char * const EmptyMessage = "";

				
                class ZMQBaseClass {
                protected:
					//!
                    int closeSocketNoWhait (void *socket);
					
					//!
					int stringReceive(void *socket, std::string& received_string);
					
					//!
					int stringSend(void *socket, const char *string);
					
					//!
					int stringSendMore(void *socket, const char *string);
					
					//!
					int setID(void *socket);
					
					//!
					int setAndReturnID(void *socket, std::string& new_id);
                };
                
            }
        }
    }
}



#endif
