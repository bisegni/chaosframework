//
//  ApiServer.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 2/26/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

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
            class ApiServer : public Singleton<ApiServer>, public utility::StartableService, private SetupStateManager {
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
