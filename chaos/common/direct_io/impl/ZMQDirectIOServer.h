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
#ifndef __CHAOSFramework__ZMQDirectIOServer__
#define __CHAOSFramework__ZMQDirectIOServer__

#include <string>

#include <chaos/common/direct_io/DirectIOServer.h>
#include <chaos/common/direct_io/impl/ZMQBaseClass.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>

#include <boost/thread.hpp>

#include <zmq.h>

namespace chaos {
    namespace common {
        namespace direct_io {
            namespace impl {
                
                typedef enum WorkerType {
                    WorkerTypePriority = 1,
                    WorkerTypeService = 2
                } WorkerType;
                
                DECLARE_CLASS_FACTORY(ZMQDirectIOServer, DirectIOServer),
                private ZMQBaseClass {
                    REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(ZMQDirectIOServer)
                    void *zmq_context;
                    unsigned int direct_io_thread_number;
                    boost::thread_group server_threads_group;
                    
                    bool run_server;
                    
                    std::string priority_socket_bind_str;
                    
                    std::string service_socket_bind_str;
                    
                    void poller(const std::string& public_url,
                                const std::string& inproc_url);
                    void worker(unsigned int w_type,
                                DirectIOHandlerPtr delegate);
                    
                    
                    ZMQDirectIOServer(std::string alias);
                    ~ZMQDirectIOServer();
                public:
                    
                    //! Initialize instance
                    void init(void *init_data) throw(chaos::CException);
                    
                    //! Start the implementation
                    void start() throw(chaos::CException);
                    
                    //! Stop the implementation
                    void stop() throw(chaos::CException);
                    
                    //! Deinit the implementation
                    void deinit() throw(chaos::CException);
                };
            }
        }
    }
}


#endif /* defined(__CHAOSFramework__ZMQDirectIOServer__) */
