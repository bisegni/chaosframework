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
