/*
 *	Startab.hleService
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2013 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
 */
#ifndef __CHAOSFramework__StartableService__
#define __CHAOSFramework__StartableService__

#include <chaos/common/utility/InizializableService.h>

namespace chaos {
    
    namespace utility {
        
        typedef enum {
            SS_STARTING = 5,
            SS_STARTED,
            SS_STOPPING,
            SS_STOPPED
        } StartableServiceState;
        
        class StartableService : public InizializableService {
        public:
            
                //! Start the implementation
            virtual void start() throw(chaos::CException) = 0;
            
                //! Start the implementation
            virtual void stop() throw(chaos::CException) = 0;
            
            static bool startImplementation(StartableService *impl, const char * const implName,  const char * const domainString);
            static bool stopImplementation(StartableService *impl, const char * const implName,  const char * const domainString);
        };
        
    }

}
#endif /* defined(__CHAOSFramework__StartableService__) */
