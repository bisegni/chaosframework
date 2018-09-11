/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */

#ifndef __CHAOSFramework__DirectIOServer__
#define __CHAOSFramework__DirectIOServer__

#include <string>
#include <map>

#include <chaos/common/utility/StartableService.h>
#include <chaos/common/utility/NamedService.h>

#include <chaos/common/direct_io/DirectIOTypes.h>
#include <chaos/common/direct_io/DirectIODispatcher.h>
#include <chaos/common/direct_io/DirectIOServerPublicInterface.h>
namespace chaos {
    namespace common {
        namespace direct_io {
            
            //! Direct IO server base class
            /*!
             This class reppresents the base interface for the operation on direct io input channel,
             The server layer will accept the client connection for the data reception. Server can accept connection
             by different client
             dio_client---> data message -->dio_server
             */
            class DirectIOServer :
            public utility::StartableService,
            public utility::NamedService,
            public DirectIOServerPublicInterface {
                //! handler implementation for the server instance
            protected:
                int32_t priority_port;
                int32_t service_port;
                std::string service_url;
                DirectIODispatcher *handler_impl;
            public:
                DirectIOServer(const std::string& alias);
                
                virtual ~DirectIOServer();
                
                // Initialize instance
                void init(void *init_data);
                
                // Start the implementation
                void start();
                
                // Stop the implementation
                void stop();
                
                // Deinit the implementation
                void deinit();
                
                //! Send some data to the server
                void setHandler(DirectIODispatcher *_handler_impl);
                
                //! Remove the handler pointer
                void clearHandler();
                
                uint32_t getPriorityPort();
                
                uint32_t getServicePort();
                
                const std::string& getUrl();
                
            };
            
        }
    }
}


#endif /* defined(__CHAOSFramework__DirectIOServer__) */
