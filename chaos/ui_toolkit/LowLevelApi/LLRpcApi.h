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

#ifndef ChaosFramework_LLRpcApi_h
#define ChaosFramework_LLRpcApi_h

#include <map>
#include <boost/shared_ptr.hpp>

#include <chaos/common/utility/Singleton.h>
#include <chaos/common/exception/exception.h>
#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/message/MDSMessageChannel.h>
#include <chaos/common/message/DeviceMessageChannel.h>

#include <chaos/common/event/channel/AlertEventChannel.h>
#include <chaos/common/event/channel/InstrumentEventChannel.h>
#include <chaos/common/io/IODirectIODriver.h>
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
             */
            LLRpcApi();
            
            /*
             */
            ~LLRpcApi();
            
        public:
            
            /*
             LL Rpc Api static initialization it should be called once for application
             */
            void init() throw (CException);
            
            /*
             * use a specified network broker
             */
            void init(chaos::common::network::NetworkBroker *network_broker);
            /*
             Deinitialization of LL rpc api
             */
            void deinit() throw (CException);
            chaos::common::io::IODataDriver *getDataProxyChannelNewInstance() throw(CException);
            
            /*!
             Return a new channel for talk with metadata server
             */
            chaos::common::message::MDSMessageChannel *getNewMetadataServerChannel();
            
            /*!
             Return a new device channel
             */
            chaos::common::message::DeviceMessageChannel *getNewDeviceMessageChannel(chaos::common::network::CDeviceNetworkAddress *device_network_address,
                                                                                     bool self_managed = false);
            
            chaos::common::message::PerformanceNodeChannel *getNewPerformanceChannel(chaos::common::network::CNetworkAddress *note_network_address);
            
            /*!
             Delete a previously instantiatedchannel
             */
            void deleteMessageChannel(chaos::common::message::MessageChannel*);
            
            /*!
             Delete a previously instantiatedchannel
             */
            void deleteMessageChannel(chaos::common::message::NodeMessageChannel*);
            
            /*!
             Delete a previously instantiatedchannel
             */
            void deleteMessageChannel(chaos::common::message::MDSMessageChannel*);
            
            chaos::common::event::channel::AlertEventChannel *getNewAlertEventChannel() throw (CException);
            chaos::common::event::channel::InstrumentEventChannel *getNewInstrumentEventChannel() throw (CException);
            void disposeEventChannel(chaos::common::event::channel::EventChannel *) throw (CException);
            
            SystemApiChannel *getSystemApiClientChannel(const std::string& direct_io_address);
            void releaseSystemApyChannel(SystemApiChannel *system_api_channel);
            
        };
    }
}
#endif
