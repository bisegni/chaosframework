/*
 *	ZMQDirectIOClientConnection.h
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

#ifndef __CHAOSFramework__ZMQDirectIOClientConnection__
#define __CHAOSFramework__ZMQDirectIOClientConnection__

#include <string>

#include <chaos/common/direct_io/DirectIODataPack.h>
#include <chaos/common/direct_io/DirectIOClientConnection.h>

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

#include <zmq.h>

namespace chaos {
	namespace common {
		namespace direct_io {
			namespace channel{
				class DirectIOVirtualClientChannel;
			}
			
            namespace impl {
				//forward declaration
				class ZMQDirectIOClient;
				
				typedef struct ConnectionMonitorInfo {
					bool run;
					std::string unique_identification;
					boost::thread *monitor_thread;
					void *monitor_socket;
					std::string monitor_url;
				} ConnectionMonitorInfo;
				
				const int _send_more_no_wait_flag = ZMQ_SNDMORE;
				const int _send_no_wait_flag = 0;
				/*!
				 Class that represetn th eimplementation of one connection of the direct io
				 connection implemented with zmq
				 */
				class ZMQDirectIOClientConnection : public chaos::common::direct_io::DirectIOClientConnection {
					friend class ZMQDirectIOClient;
					
					void *socket_priority;
					void *socket_service;
					
					ConnectionMonitorInfo *monitor_info;
					
                    boost::shared_mutex mutex_socket_manipolation;
					
					ZMQDirectIOClientConnection(std::string server_description, void *_socket_priority, void *_socket_service, uint16_t endpoint);
					~ZMQDirectIOClientConnection();
					
					inline int64_t writeToSocket(channel::DirectIOVirtualClientChannel *channel, void *socket, DirectIODataPack *data_pack);
					void monitorWorker();
					
				protected:
					
                    // send the data to the server layer on priority channel
                    int64_t sendPriorityData(channel::DirectIOVirtualClientChannel *channel, DirectIODataPack *data_pack);
                    
                    // send the data to the server layer on the service channel
                    int64_t sendServiceData(channel::DirectIOVirtualClientChannel *channel, DirectIODataPack *data_pack);
					
				public:
					int addServer(std::string server_description);
					int removeServer(std::string server_description);
				};
				
			}
		}
	}
}

#endif /* defined(__CHAOSFramework__ZMQDirectIOClientConnection__) */
