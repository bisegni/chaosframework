/*
 *	MetadataServiceClient.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
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
#ifndef __CHAOSFramework__MetadataServiceClient__
#define __CHAOSFramework__MetadataServiceClient__

#include <ChaosMetadataServiceClient/api_proxy/api.h>
#include <ChaosMetadataServiceClient/metadata_service_client_types.h>
#include <ChaosMetadataServiceClient/monitor_system/MonitorManager.h>

#include <boost/thread/condition.hpp>

#include <chaos/common/global.h>
#include <chaos/common/ChaosCommon.h>
#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/thread/WaitSemaphore.h>
#include <chaos/common/utility/StartableService.h>

namespace chaos {
    namespace metadata_service_client {
        //! Chaos Node Directory base class
        /*!
         Singleton class that act as main entry for mds client library
         */
        class ChaosMetadataServiceClient :
        public ChaosCommon<ChaosMetadataServiceClient>,
        public ServerDelegator {
            friend class common::utility::Singleton<ChaosMetadataServiceClient>;
            
            //!network broker service
            common::utility::StartableServiceContainer<chaos::common::network::NetworkBroker> network_broker_service;
            
            //!api proxy service
            common::utility::InizializableServiceContainer<chaos::metadata_service_client::api_proxy::ApiProxyManager> api_proxy_manager;
            
            //!monitor manager
            common::utility::StartableServiceContainer<chaos::metadata_service_client::monitor_system::MonitorManager> monitor_manager;
            
            //!default constructor
            ChaosMetadataServiceClient();
            
            //! default destructor
            ~ChaosMetadataServiceClient();
            
            //! set the custom client init parameter
            void setClientInitParameter();
        public:
            //! the client setting
            ClientSetting setting;
            
            //! C and C++ attribute parser
            /*!
             Specialized option for startup c and cpp program main options parameter
             */
            void init(int argc, char* argv[]) throw (CException);
            
            //! StartableService inherited method
            void init(void *init_data)  throw(CException);
            //! StartableService inherited method
            void start()throw(CException);
            //! StartableService inherited method
            void stop()throw(CException);
            //! StartableService inherited method
            void deinit()throw(CException);
            
            template<typename D>
            D* getApiProxy(int32_t timeout_in_milliseconds = 1000) {
                return api_proxy_manager->getApiProxy<D>(timeout_in_milliseconds);
            }
            
            void clearServerList();
            
            void addServerAddress(const std::string& server_address_and_port);
            
            void enableMonitoring() throw(CException);
            
            void disableMonitoring() throw(CException);
            
            //! add a new quantum slot for key
            bool addKeyConsumer(const std::string& key_to_monitor,
                                int quantum_multiplier,
                                monitor_system::QuantumSlotConsumer *consumer,
                                int consumer_priority = 500);
            
            //! remove a consumer by key and quantum
            bool removeKeyConsumer(const std::string& key_to_monitor,
                                   int quantum_multiplier,
                                   monitor_system::QuantumSlotConsumer *consumer);
        };
    }
}

#endif /* defined(__CHAOSFramework__MetadataServiceClient__) */
