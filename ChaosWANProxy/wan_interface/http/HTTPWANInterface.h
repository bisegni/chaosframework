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

#ifndef __CHAOSFramework__HTTPWANInterface__
#define __CHAOSFramework__HTTPWANInterface__

#include "../AbstractWANInterface.h"

#include <chaos/common/utility/ObjectFactoryRegister.h>

#include <boost/thread.hpp>
#include <boost/atomic.hpp>

#include "mongoose.h"

namespace chaos {
    
    //forward declaration
    namespace common {
        namespace data {
            class CDataWrapper;
        }
    }
    
    namespace wan_proxy {
        namespace wan_interface {
            namespace http {
                typedef std::vector<mongoose::mg_server *>				ServerList;
                typedef std::vector<mongoose::mg_server *>::iterator	ServerListIterator;
                
                //!interface param key
                static const char * const	OPT_HTTP_PORT			= "HTTP_wi_port";
                //!
                static const char * const	OPT_HTTP_THREAD_NUMBER  = "HTTP_wi_thread_number";
                /*
                 Class that implement the Chaos RPC server using HTTP
                 */
                DECLARE_CLASS_FACTORY(HTTPWANInterface, AbstractWANInterface) {
                    REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(HTTPWANInterface)
                    
                    HTTPWANInterface(const string& alias);
                    ~HTTPWANInterface();
                    
                    bool run;
                    
                    int thread_number;
                    
                    ServerList http_server_list;
                    
                    boost::atomic<int> thread_index;
                    
                    boost::thread_group http_server_thread;
                    std::string basePath;
                    //!poll the http server in a thread
                    void pollHttpServer(mongoose::mg_server *http_server);
                    bool checkForContentType(mongoose::mg_connection *connection,
                                             const std::string& type);
                public:
                    int process(mongoose::mg_connection *connection);
                    bool handle(mongoose::mg_connection *connection);
                protected:
                    
                    //inherited method
                    void init(void*) throw(CException);
                    
                    //inherited method
                    void start() throw(CException);
                    
                    //inherited method
                    void stop() throw(CException);
                    
                    //inherited method
                    void deinit() throw(CException);
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__HTTPWANInterface__) */
