/*
 *	ApiServer.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__ApiServer__
#define __CHAOSFramework__ApiServer__

#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/utility/Singleton.h>
#include <chaos/common/exception/CException.h>
#include <chaos/common/utility/SetupStateManager.h>
#include <chaos/common/utility/StartableService.h>

#include "ApiProvider.h"
#include "../data/DataManagment.h"

namespace chaos {
    
    namespace cnd {
        namespace api {
            
            //! ApiServer
            /*!
             
             */
            class ApiServer :
			public Singleton<ApiServer>,
			public utility::StartableService {
                friend class Singleton<ApiServer>;
                chaos::cnd::data::DataManagment *dm;
                NetworkBroker *networkBroker;
                
            public:
                
                /*!
                 */
                ApiServer();
                
                /*!
                 */
                ~ApiServer();
                
                // Initialize instance
                void init(void*) throw(chaos::CException);
                
                // Start the implementation
                void start() throw(chaos::CException);
    
                // Start the implementation
                void stop() throw(chaos::CException);
                
                // Deinit the implementation
                void deinit() throw(chaos::CException);
                
                /*!
                 Register a class that define some api
                 */
                void registerApi(ApiProvider *apiClass);
                
                /*!
                 Deeregister a class that define some api
                 */
                void deregisterApi(ApiProvider *apiClass);
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__ApiServer__) */
