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


#include <boost/shared_ptr.hpp>

#include <chaos/common/utility/Singleton.h>
#include <chaos/common/exception/CException.h>
#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/message/MDSMessageChannel.h>
#include <chaos/common/message/DeviceMessageChannel.h>
#include <chaos/common/utility/SetupStateManager.h>
#include <chaos/common/event/channel/AlertEventChannel.h>
#include <chaos/common/event/channel/InstrumentEventChannel.h>
#include <chaos/common/io/IODirectIODriver.h>
#include <chaos/common/io/IOMemcachedIODriver.h>
namespace chaos {
    namespace ui{
        using namespace boost;  
        
        /*
         LLRpcApi Class api for rpc system
         */
        class LLRpcApi : public Singleton<LLRpcApi>, private SetupStateManager {
            friend class ChaosUIToolkit;
            friend class Singleton<LLRpcApi>;
            
            chaos::NetworkBroker *network_broker;
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
            IODataDriver *getDataProxyChannelNewInstance() throw(CException);
			
            /*!
             Return a new channel for talk with metadata server
             */
            MDSMessageChannel *getNewMetadataServerChannel();
            
            /*!
             Return a new device channel
             */
            DeviceMessageChannel *getNewDeviceMessageChannel(CDeviceNetworkAddress *deviceNetworkAddress);
            
            /*!
             Delete a previously instantiatedchannel
             */
            void deleteMessageChannel(NodeMessageChannel*);
            
            event::channel::AlertEventChannel *getNewAlertEventChannel() throw (CException);
            event::channel::InstrumentEventChannel *getNewInstrumentEventChannel() throw (CException);
            void disposeEventChannel(event::channel::EventChannel *) throw (CException);
        };
    }
}
#endif
