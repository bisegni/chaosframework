/*
 *	ZMQSocketFeeder.h
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
#ifndef __CHAOSFramework__ZMQSocketFeeder__
#define __CHAOSFramework__ZMQSocketFeeder__

#include <map>

#include <boost/thread.hpp>
#include <boost/lockfree/queue.hpp>

namespace chaos {
	namespace common {
		namespace direct_io {
            namespace impl {
				
				typedef struct endpoint_dec {
					char		priority[255];
					char		service[255];
				} endpoint_dec;
				
				typedef struct socket_info {
					uint16_t			index;
					uint32_t			hash_host_ip;
					void				*socket;
					endpoint_dec		endpoints;
					uint64_t			last_online_ts;
					uint32_t			check_priority;
					boost::shared_mutex socket_mutex;
				} socket_info;
				
				class ZMQSocketFeeder {
					
					uint16_t socket_allocated;
					socket_info ** socket_slot;
					
					boost::lockfree::queue<uint16_t> online_socket;
					boost::lockfree::queue<uint16_t> offline_socket;
			
					void decoupleServerDescription(std::string server_desc, socket_info *socket_info);
				public:
					ZMQSocketFeeder();
					~ZMQSocketFeeder();
					void *getNextSocket();
					bool addServer(std::string server_desc);
				};
				
			}
		}
	}
}

#endif /* defined(__CHAOSFramework__ZMQSocketFeeder__) */
