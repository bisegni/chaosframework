/*
 *	HTTPUIInterface.h
 *	!CHAOS
 *	Created by Andrea Michelotti
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

#ifndef __CHAOSFramework__HTTPUIInterface__
#define __CHAOSFramework__HTTPUIInterface__
#include <driver/misc/core/ChaosController.h>
#include "../AbstractWANInterface.h"

#include <chaos/common/utility/ObjectFactoryRegister.h>

#include <boost/thread.hpp>
#include <boost/atomic.hpp>
#include <boost/thread/mutex.hpp>
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
                typedef std::vector<struct mg_server *>				ServerList;
                typedef std::vector<struct mg_server *>::iterator	ServerListIterator;
                
                //!interface param key
                static const char * const	OPT_HTTP_PORT			= "HTTP_wi_port";
                //!
                static const char * const	OPT_HTTP_THREAD_NUMBER  = "HTTP_wi_thread_number";
                /*
                 Class that implement the Chaos RPC server using HTTP
                 */
                DECLARE_CLASS_FACTORY(HTTPUIInterface, AbstractWANInterface) {
                    REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(HTTPUIInterface)
                    
                    HTTPUIInterface(const string& alias);
                    ~HTTPUIInterface();
                    
                    bool run;
                    
                    int thread_number;
                    
                    ServerList http_server_list;
                    
                    boost::atomic<int> thread_index;
                    
                    boost::thread_group http_server_thread;
                    std::string basePath;
                    //!poll the http server in a thread
                    void pollHttpServer(struct mg_server *http_server);
                    static std::map<std::string,::driver::misc::ChaosController*> devs;

                     ::driver::misc::ChaosController* info;
                    static void addDevice(std::string,::driver::misc::ChaosController*);
                    static void removeDevice(std::string);
                    bool checkForContentType(struct mg_connection *connection,
                                                               const std::string& type);
                    boost::mutex devio_mutex;
                public:
                    bool handle(struct mg_connection *connection);

                    int process(struct mg_connection *connection);
                    int processRest(struct mg_connection *connection);

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

#endif /* defined(__CHAOSFramework__HTTPUIInterface__) */
