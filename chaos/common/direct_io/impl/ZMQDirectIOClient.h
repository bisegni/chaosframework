
/*
 *	ZMQDirectIOClient.h
 *	!CHAOS
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
#ifndef __CHAOSFramework__ZMQDirectIOClient__
#define __CHAOSFramework__ZMQDirectIOClient__

#include <map>

#include <chaos/common/direct_io/DirectIOClient.h>
#include <chaos/common/direct_io/impl/ZMQBaseClass.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/direct_io/impl/ZMQDirectIOClientConnection.h>

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

#include <zmq.h>

namespace chaos {
	namespace common {
		namespace direct_io {
			namespace channel {
				class DirectIOVirtualClientChannel;
			}
            namespace impl {
				
				// ZMQ Direct IO Implementation
				/*!
				 */
                DECLARE_CLASS_FACTORY(ZMQDirectIOClient, DirectIOClient),
                private ZMQBaseClass {
                    REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(ZMQDirectIOClient)

					friend class DirectIOVirtualClientChannel;
					
                    int32_t priority_port;
					
					int32_t service_port;

					void *zmq_context;
					
					bool thread_run;
					boost::thread_group monitor_thread_group;
                    
					std::map<uint32_t, ConnectionMonitorInfo*> map_connection_socket_monitor;
					
					ZMQDirectIOClient(std::string alias);
                    
                    ~ZMQDirectIOClient();
					
				protected:
					int readMesg(void* s, zmq_event_t* event, char* ep);
					void *socketMonitor(void *s, const char * address, ConnectionMonitorInfo *monitor_info);
					//overriding ofr free object fuunction for the tempalted key object container superclass
                    void freeObject(const DCKeyObjectContainer::TKOCElement& element);
                    
                    //! get new connection
                    DirectIOClientConnection *_getNewConnectionImpl(std::string server_description, uint16_t endpoint);
                    
                    //! release an instantiated connection
                    void _releaseConnectionImpl(DirectIOClientConnection *connection_to_release);
                public:
                    
                    //! Initialize instance
                    void init(void *init_data) throw(chaos::CException);

                    
                    //! Deinit the implementation
                    void deinit() throw(chaos::CException);
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__DirectIOZMQClient__) */
