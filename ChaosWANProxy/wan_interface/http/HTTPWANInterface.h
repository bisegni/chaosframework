/*
 *	HTTPWANInterface.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2014 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__HTTPWANInterface__
#define __CHAOSFramework__HTTPWANInterface__

#include "../AbstractWANInterface.h"

#include <chaos/common/utility/ObjectFactoryRegister.h>

#include <boost/thread.hpp>
#include <boost/atomic.hpp>

#include <mongoose.h>

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
                typedef std::vector<struct mg_server *>				ServerList;
                typedef std::vector<struct mg_server *>::iterator	ServerListIterator;
                
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
                    
                    //!poll the http server in a thread
                    void pollHttpServer(struct mg_server *http_server);
                    bool checkForContentType(struct mg_connection *connection,
                                             const std::string& type);
                public:
                    int process(struct mg_connection *connection);
                    bool handle(struct mg_connection *connection);
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
