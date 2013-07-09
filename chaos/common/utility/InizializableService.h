/*
 *	InizializableService.h
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

#ifndef __CHAOSFramework__InizializableService__
#define __CHAOSFramework__InizializableService__

#include <inttypes.h>

#include <chaos/common/exception/CException.h>

namespace chaos {
    namespace utility {
        namespace InizializableServiceType {
            typedef enum {
                IS_INITING,
                IS_INITIATED,
                IS_DEINITING,
                IS_DEINTIATED
            } InizializableServiceState;
        }
        class InizializableService {
        protected:
            uint8_t serviceState;
        public:
            InizializableService();
            virtual ~InizializableService();
                //! Return the state
            const uint8_t getServiceState() const;
            
                //! Initialize instance
            virtual void init(void*) throw(chaos::CException);
            
                //! Deinit the implementation
            virtual void deinit() throw(chaos::CException);
            
            static bool initImplementation(InizializableService *impl, void *initData, const char * const implName,  const char * const domainString);
            static bool deinitImplementation(InizializableService *impl, const char * const implName,  const char * const domainString);
        };
        
    }
}

#endif /* defined(__CHAOSFramework__InizializableService__) */
