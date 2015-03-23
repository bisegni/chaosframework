/*
 *	LLRpcApi.h
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

#ifndef ChaosFramework_LLRpcApi_h
#define ChaosFramework_LLRpcApi_h

#include <map>
#include <boost/shared_ptr.hpp>

#include <chaos/common/utility/Singleton.h>
#include <chaos/common/exception/CException.h>
#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/message/MDSMessageChannel.h>
#include <chaos/common/message/DeviceMessageChannel.h>

#include <chaos/common/event/channel/AlertEventChannel.h>
#include <chaos/common/event/channel/InstrumentEventChannel.h>
#include <chaos/common/io/IODirectIODriver.h>
#include <chaos/common/io/IOMemcachedIODriver.h>
#include <chaos/common/direct_io/DirectIOClient.h>
#include <chaos/common/direct_io/DirectIOClientConnection.h>
#include <chaos/common/direct_io/channel/DirectIOSystemAPIClientChannel.h>

#include <boost/thread.hpp>
#include <boost/atomic.hpp>

namespace chaos {
	namespace ui{
		
		class LLRpcApi;
		
		struct DIOConn {
			friend class LLRpcApi;
		private:
			static uint32_t garbage_counter;
            chaos::common::direct_io::DirectIOClientConnection *connection;
			DIOConn(chaos::common::direct_io::DirectIOClientConnection *_connection):connection(_connection){};
			~DIOConn(){};
		};
		
		struct SystemApiChannel {
			friend class LLRpcApi;
			chaos::common::direct_io::channel::DirectIOSystemAPIClientChannel *system_api_channel;
		private:
			DIOConn *connection;
			SystemApiChannel(DIOConn *_connection,
							 chaos::common::direct_io::channel::DirectIOSystemAPIClientChannel *_system_api_channel):
			connection(_connection),
			system_api_channel(_system_api_channel){};
			~SystemApiChannel(){};
		};
		
		/*
		 LLRpcApi Class api for rpc system
		 */
		class LLRpcApi:
		public common::utility::Singleton<LLRpcApi> {
			friend class ChaosUIToolkit;
			friend class common::utility::Singleton<LLRpcApi>;
			//!chaos network router
			chaos::common::network::NetworkBroker *network_broker;
			
			//! root direct io client
			chaos::common::direct_io::DirectIOClient *direct_io_client;
			
			//! hasmap for direct io address and connection struct
			boost::mutex				mutex_map_dio_addr_conn;
            std::map<std::string, DIOConn*>	map_dio_addr_conn;
			
			/*
			 LL Rpc Api static initialization it should be called once for application
			 */
			void init() throw (CException);
			/*
			 Deinitialization of LL rpc api
			 */
			void deinit() throw (CException);
			
			/*
			 */
			LLRpcApi();
			
			/*
			 */
			~LLRpcApi();
			
		public:
			chaos::common::io::IODataDriver *getDataProxyChannelNewInstance() throw(CException);
			
			/*!
			 Return a new channel for talk with metadata server
			 */
            chaos::common::message::MDSMessageChannel *getNewMetadataServerChannel();
			
			/*!
			 Return a new device channel
			 */
            chaos::common::message::DeviceMessageChannel *getNewDeviceMessageChannel(chaos::common::network::CDeviceNetworkAddress *device_network_address);
			
            chaos::common::message::PerformanceNodeChannel *getNewPerformanceChannel(chaos::common::network::CNetworkAddress *note_network_address);
			
			/*!
			 Delete a previously instantiatedchannel
			 */
            void deleteMessageChannel(chaos::common::message::MessageChannel*);
			
			/*!
			 Delete a previously instantiatedchannel
			 */
            void deleteMessageChannel(chaos::common::message::NodeMessageChannel*);
			
            chaos::event::channel::AlertEventChannel *getNewAlertEventChannel() throw (CException);
            chaos::event::channel::InstrumentEventChannel *getNewInstrumentEventChannel() throw (CException);
            void disposeEventChannel(chaos::event::channel::EventChannel *) throw (CException);
			
            SystemApiChannel *getSystemApiClientChannel(const std::string& direct_io_address);
			void releaseSystemApyChannel(SystemApiChannel *system_api_channel);
			
		};
	}
}
#endif
