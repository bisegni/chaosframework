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
namespace chaos_io = chaos::common::io;
namespace chaos_network = chaos::common::network;
namespace chaos_direct_io = chaos::common::direct_io;

namespace chaos {
    namespace ui{
		
		class LLRpcApi;

		struct DIOConn {
			friend class LLRpcApi;
		private:
			static uint32_t garbage_counter;
			chaos_direct_io::DirectIOClientConnection *connection;
			DIOConn(chaos_direct_io::DirectIOClientConnection *_connection):connection(_connection){};
			~DIOConn(){};
		};
		
		struct SystemApiChannel {
			friend class LLRpcApi;
			chaos_direct_io::channel::DirectIOSystemAPIClientChannel *system_api_channel;
		private:
			DIOConn *connection;
			SystemApiChannel(DIOConn *_connection,
							 chaos_direct_io::channel::DirectIOSystemAPIClientChannel *_system_api_channel):
			connection(_connection),
			system_api_channel(_system_api_channel){};
			~SystemApiChannel(){};
		};
		
        /*
         LLRpcApi Class api for rpc system
         */
        class LLRpcApi:
		public common::utility::Singleton<LLRpcApi>,
		private SetupStateManager {
            friend class ChaosUIToolkit;
            friend class Singleton<LLRpcApi>;
            //!chaos network router
            chaos_network::NetworkBroker *network_broker;
			
			//! root direct io client
			chaos_direct_io::DirectIOClient *direct_io_client;
			
			//! hasmap for direct io address and connection struct
			boost::mutex				mutex_map_dio_addr_conn;
			std::map<string, DIOConn*>	map_dio_addr_conn;
			
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
			chaos_io::IODataDriver *getDataProxyChannelNewInstance() throw(CException);
			
            /*!
             Return a new channel for talk with metadata server
             */
            MDSMessageChannel *getNewMetadataServerChannel();
            
            /*!
             Return a new device channel
             */
            DeviceMessageChannel *getNewDeviceMessageChannel(CDeviceNetworkAddress *deviceNetworkAddress);
            
			chaos::common::message::PerformanceNodeChannel *getNewPerformanceChannel(CNetworkAddress *note_network_address);
			
            /*!
             Delete a previously instantiatedchannel
             */
            void deleteMessageChannel(MessageChannel*);
            
			/*!
             Delete a previously instantiatedchannel
             */
            void deleteMessageChannel(NodeMessageChannel*);
			
            event::channel::AlertEventChannel *getNewAlertEventChannel() throw (CException);
            event::channel::InstrumentEventChannel *getNewInstrumentEventChannel() throw (CException);
            void disposeEventChannel(event::channel::EventChannel *) throw (CException);
			
			SystemApiChannel *getSystemApiClientChannel(const std::string& direct_io_address);
			void releaseSystemApyChannel(SystemApiChannel *system_api_channel);
			
        };
    }
}
#endif
