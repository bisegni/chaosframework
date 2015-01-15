/*
 *	ChaosBridge.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyrigh 2015 INFN, National Institute of Nuclear Physics
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
#ifndef __CHAOSFramework__ChaosBridge__
#define __CHAOSFramework__ChaosBridge__

#include <vector>
#include <string>

#include <chaos/common/message/MDSMessageChannel.h>
#include <chaos/common/direct_io/DirectIOClient.h>
#include <chaos/common/direct_io/DirectIOClientConnection.h>
#include <chaos/common/network/URLServiceFeeder.h>
#include <chaos/common/data/CDataWrapper.h>

#include <boost/thread.hpp>

namespace chaos {
	namespace wan_proxy {
		
		class ChaosWANProxy;
		
		typedef struct DirectIOChannelsInfo {
			boost::shared_mutex													connection_mutex;
			chaos::common::direct_io::DirectIOClientConnection					*connection;
			chaos::common::direct_io::channel::DirectIODeviceClientChannel		*device_client_channel;
		} DirectIOChannelsInfo;
		
		//! class that realize the bridget versus cds and metadataserver
		class ChaosBridge:
		protected common::network::URLServiceFeederHandler,
		protected chaos::common::direct_io::DirectIOClientConnectionEventHandler {
			friend class ChaosWANProxy;
			chaos::common::direct_io::DirectIOClient	*direct_io_client;
			chaos::common::message::MDSMessageChannel	*mds_messahe_channel;
			
			chaos::common::network::URLServiceFeeder	connection_feeder;
			
		protected:
			//!inherited by @common::network::URLServiceFeederHandler
			void  disposeService(void *service_ptr);
			
			//! inherited by @common::network::URLServiceFeederHandler
			void* serviceForURL(const URL& url, uint32_t service_index);
			
			//! inherited by @chaos::common::direct_io::DirectIOClientConnectionEventHandler
			void handleEvent(chaos_direct_io::DirectIOClientConnection *client_connection,
							 chaos_direct_io::DirectIOClientConnectionStateType::DirectIOClientConnectionStateType event);
		public:
			ChaosBridge(const std::vector<std::string>&				cds_address_list,
						chaos::common::direct_io::DirectIOClient	*_direct_io_client);
			~ChaosBridge();
			
			void clear();
			
			//! push a dataset
			void pushDataset(const std::string& producer_key,
							 chaos::common::data::CDataWrapper *dataset,
							 int store_hint);
			
			//! get a dataset
			chaos::common::data::CDataWrapper *getDataset(const std::string& producer_key);
		};
		
	}
}

#endif /* defined(__CHAOSFramework__ChaosBridge__) */
