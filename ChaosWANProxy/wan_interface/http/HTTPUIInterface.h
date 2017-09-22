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

#ifndef __CHAOSFramework__HTTPUIInterface__
#define __CHAOSFramework__HTTPUIInterface__
#include <driver/misc/core/ChaosController.h>
#include "../AbstractWANInterface.h"

#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <common/misc/scheduler/Scheduler.h>
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
                typedef std::vector<mongoose::mg_server *>				ServerList;
                typedef std::vector<mongoose::mg_server *>::iterator	ServerListIterator;
                
                //!interface param key
                static const char * const	OPT_HTTP_PORT			= "HTTP_wi_port";
                //!
                static const char * const	OPT_HTTP_THREAD_NUMBER  = "HTTP_wi_thread_number";
                static const char * const	OPT_CHAOS_THREAD_NUMBER  = "CHAOS_thread_number";

                /*
                 Class that implement the Chaos RPC server using HTTP
                 */
                DECLARE_CLASS_FACTORY(HTTPUIInterface, AbstractWANInterface) {
                    REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(HTTPUIInterface)
                    
                    HTTPUIInterface(const string& alias);
                    ~HTTPUIInterface();
                    
                    bool run;
                    
                    int thread_number;
                    int chaos_thread_number;
                    ServerList http_server_list;
                    
                    boost::atomic<int> thread_index;
                    
                    boost::thread_group http_server_thread;
                    std::string basePath;
                    //!poll the http server in a thread
                    void pollHttpServer(mongoose::mg_server *http_server);
                    static std::map<std::string,::driver::misc::ChaosController*> devs;

                     ::driver::misc::ChaosController* info;
                     std::vector< ::common::misc::scheduler::Scheduler* > sched_cu_v;
                     int sched_alloc;

                    static void addDevice(std::string,::driver::misc::ChaosController*);
                    static void removeDevice(std::string);
                    bool checkForContentType(mongoose::mg_connection *connection,
                                                               const std::string& type);
                    boost::mutex devio_mutex;
                public:
                    bool handle(mongoose::mg_connection *connection);

                    int process(mongoose::mg_connection *connection);
                    int processRest(mongoose::mg_connection *connection);

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
