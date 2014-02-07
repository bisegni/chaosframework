/*
 *	DirectIOClient.h
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
#ifndef __CHAOSFramework__DirectIOClient__
#define __CHAOSFramework__DirectIOClient__

#include <map>
#include <string>
#include <inttypes.h>

#include <chaos/common/utility/ObjectInstancer.h>
#include <chaos/common/utility/StartableService.h>
#include <chaos/common/utility/NamedService.h>

#include <chaos/common/direct_io/ServerFeeder.h>
#include <chaos/common/direct_io/DirectIOTypes.h>
#include <chaos/common/direct_io/DirectIOForwarder.h>

#include <boost/thread.hpp>
#include <boost/atomic/atomic.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <chaos/common/data/cache/FastHash.h>



namespace chaos {
	namespace common {
		namespace direct_io {
			
            namespace channel {
                class DirectIOVirtualClientChannel;
            }
            
			//! Direct IO client base class
			/*!
				This class represent the base interface for the operation on direct io output channel,
				The client layer will connect to the server one to send data. Client can connect to 
				may server and the same data will be forwarded to all server
				dio_client---> data message -->dio_server
			 */
			class DirectIOClient : public DirectIOForwarder, public chaos::utility::StartableService, public NamedService {
                std::string impl_alias;
                
                boost::atomic_uint channel_counter;
                
                boost::ptr_vector<DirectIOConnection> connections_info;
                
                boost::shared_mutex mutex_channel_map;
                std::map<unsigned int, channel::DirectIOVirtualClientChannel* > channel_map;
                
                //! release all channel and instancer
                void clearChannelInstancerAndInstance();
                

            protected:
                //! server managment utility
                ServerFeeder server_managment;
                
                uint32_t current_server_hash;
                
                std::string current_priority_endpoint;
				
				std::string current_service_endpoint;
                //!current spread type
                DirectIOConnectionSpreadType::DirectIOConnectionSpreadType current_spread_forwarding_type;
                
                virtual void switchMode(DirectIOConnectionSpreadType::DirectIOConnectionSpreadType direct_io_spread_mode) = 0;
			public:
                DirectIOClient(string alias);
				
				virtual ~DirectIOClient();
                
				//! Initialize instance
				void init(void *init_data) throw(chaos::CException);
				
				//! Start the implementation
				void start() throw(chaos::CException);
				
				//! Stop the implementation
				void stop() throw(chaos::CException);
				
				//! Deinit the implementation
				void deinit() throw(chaos::CException);
				
                //! Initialize instance
				void updateConfiguration(void *init_data) throw(chaos::CException);
                
                //! Add a new channel instantiator
                channel::DirectIOVirtualClientChannel *registerChannelInstance(channel::DirectIOVirtualClientChannel *channel_instance);
                
                //! Dispose the channel
                void deregisterChannelInstance(channel::DirectIOVirtualClientChannel *channel_instance);
			};
			
		}
	}
}

#endif /* defined(__CHAOSFramework__DirectIOClient__) */
