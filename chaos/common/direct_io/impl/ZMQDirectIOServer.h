/*
 *	ZMQDirectIOServer.h
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
				
                REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY(ZMQDirectIOServer, DirectIOServer) , private ZMQBaseClass {
                    REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(ZMQDirectIOServer)
                    void *zmq_context;
                    
                    void *priority_socket;
                    
                    void *service_socket;
                    
                    boost::thread_group server_threads_group;
                    
                    bool run_server;
                    
                    std::string priority_socket_bind_str;
                    
                    std::string service_socket_bind_str;
                    
                    void worker(bool priority_service);
					
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
