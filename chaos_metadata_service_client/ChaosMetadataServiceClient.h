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
#ifndef __CHAOSFramework__MetadataServiceClient__
#define __CHAOSFramework__MetadataServiceClient__

//#include <chaos_metadata_service_client/node_controller/CUController.h>
#include <chaos_metadata_service_client/event/event.h>
#include <chaos_metadata_service_client/api_proxy/api.h>
#include <chaos_metadata_service_client/metadata_service_client_types.h>
#include <chaos_metadata_service_client/monitor_system/monitor_system.h>
#include <chaos_metadata_service_client/node_monitor/node_monitor.h>
#include <chaos_metadata_service_client/node_controller/CUController.h>

#include <boost/thread/condition.hpp>
#include <chaos/common/global.h>
#include <chaos/common/ChaosCommon.h>
#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/thread/WaitSemaphore.h>
#include <chaos/common/utility/StartableService.h>
#include <chaos/common/io/IODataDriver.h>

namespace chaos {
    namespace metadata_service_client {


        //! Chaos Node Directory base class
        /*!
         Singleton class that act as main entry for mds client library
         */
        class ChaosMetadataServiceClient:
        public ChaosCommon<ChaosMetadataServiceClient>,
        public ServerDelegator{
            friend class common::utility::Singleton<ChaosMetadataServiceClient>;
            //!api proxy service
            common::utility::InizializableServiceContainer<chaos::metadata_service_client::api_proxy::ApiProxyManager> api_proxy_manager;
            
            //!event dispatch service
            common::utility::InizializableServiceContainer<chaos::metadata_service_client::event::EventDispatchManager> event_dispatch_manager;
            
            //!monitor manager
            common::utility::StartableServiceContainer<chaos::metadata_service_client::monitor_system::MonitorManager> monitor_manager;
            
            //!api proxy service
            common::utility::InizializableServiceContainer<chaos::metadata_service_client::node_monitor::NodeMonitor> node_monitor;
            
            //!default constructor
            ChaosMetadataServiceClient();
            
            //! default destructor
            ~ChaosMetadataServiceClient();
            bool mds_client_initialized,mds_client_deinitialized;
            std::vector<chaos::common::io::IODataDriverShrdPtr> iopool;
            int io_pool_req;

        public:
            //! the client setting
            ClientSetting setting;
            
            //! C and C++ attribute parser
            /*!
             Specialized option for startup c and cpp program main options parameter
             */
            void init(int argc, const char* argv[]) ;

            //! For common methods already initialized
            void init() ;
            //! StartableService inherited method
            void init(void *init_data) ;
            //! StartableService inherited method
            void start();
            //! StartableService inherited method
            void stop();
            //! StartableService inherited method
            void deinit();
            
            template<typename D>
            ChaosUniquePtr<D> getApiProxy(int32_t timeout_in_milliseconds = RpcConfigurationKey::GlobalRPCTimeoutinMSec) {
                return api_proxy_manager->getApiProxy<D>(timeout_in_milliseconds);
            }
            
            void clearServerList();
            
            void addServerAddress(const std::string& server_address_and_port);
            
            void enableMonitor();
            
            void disableMonitor();
            
            void reconfigureMonitor();
            
            bool monitoringIsStarted();
            
            bool addHandlerToNodeMonitor(const std::string& node_uid,
                                         node_monitor::ControllerType controller_type,
                                         node_monitor::NodeMonitorHandler *handler_to_add);
            
            bool removeHandlerToNodeMonitor(const std::string& node_uid,
                                            node_monitor::ControllerType controller_type,
                                            node_monitor::NodeMonitorHandler *handler_to_remove);
            
            //! add a new quantum slot for key
            bool addKeyConsumer(const std::string& key_to_monitor,
                                int quantum_multiplier,
                                monitor_system::QuantumSlotConsumer *consumer,
                                int consumer_priority = 500);
            
            //! add a new consumer ofr the healt data associated to a key
            bool addKeyConsumerForHealt(const std::string& key_to_monitor,
                                        int quantum_multiplier,
                                        monitor_system::QuantumSlotConsumer *consumer,
                                        int consumer_priority = 500);
            
            //! add a new handler for a single attribute for a key
            bool addKeyAttributeHandler(const std::string& key_to_monitor,
                                        int quantum_multiplier,
                                        chaos::metadata_service_client::monitor_system::AbstractQuantumKeyAttributeHandler *attribute_handler,
                                        unsigned int consumer_priority = 500);
            
            //! add a new handler for a single attribute for a healt data for a key
            bool addKeyAttributeHandlerForHealt(const std::string& key_to_monitor,
                                                int quantum_multiplier,
                                                monitor_system::AbstractQuantumKeyAttributeHandler *attribute_handler,
                                                unsigned int consumer_priority = 500);
            
            bool addKeyAttributeHandlerForDataset(const std::string& key_to_monitor,
                                                  const unsigned int dataset_type,
                                                  int quantum_multiplier,
                                                  monitor_system::AbstractQuantumKeyAttributeHandler *attribute_handler,
                                                  unsigned int consumer_priority = 500);
            
            //! Remove a consumer by key and quantum
            /*!
             The remove operation can be also executed specifind false on
             wait_completion parameter. In this case The scheduler try to
             remove the consumer but if it is in use, the remove operation
             will be submitted to the asynchronous layer, so the caller
             neet to call the waitForCompletion of the consumer to
             waith that the remove operationhas been terminated
             \param key_to_monitor the key to monitor
             \param quantum_multiplier is the quantum multipier that will determinate
             the delay form a data request and nother
             \param consumer the pointer of the consumer that need to be notified
             \param wait_completion detarminate if the called whant to wait the completion
             of the operation or whant check by itself
             \return true if the consumer has been removed
             */
            bool removeKeyConsumer(const std::string& key_to_monitor,
                                   int quantum_multiplier,
                                   monitor_system::QuantumSlotConsumer *consumer,
                                   bool wait_completion = true);
            
            //! Remove a consumer for healt dataset by key and quantum
            /*!
             The remove operation can be also executed specifind false on
             wait_completion parameter. In this case The scheduler try to
             remove the consumer but if it is in use, the remove operation
             will be submitted to the asynchronous layer, so the caller
             neet to call the waitForCompletion of the consumer to
             waith that the remove operationhas been terminated
             \param key_to_monitor the key to monitor
             \param quantum_multiplier is the quantum multipier that will determinate
             the delay form a data request and nother
             \param consumer the pointer of the consumer that need to be notified
             \param wait_completion detarminate if the called whant to wait the completion
             of the operation or whant check by itself
             \return true if the consumer has been removed
             */
            bool removeKeyConsumerForHealt(const std::string& key_to_monitor,
                                           int quantum_multiplier,
                                           monitor_system::QuantumSlotConsumer *consumer,
                                           bool wait_completion = true);
            
            //! remove an handler associated to ans attirbute of a key
            bool removeKeyAttributeHandler(const std::string& key_to_monitor,
                                           int quantum_multiplier,
                                           monitor_system::AbstractQuantumKeyAttributeHandler *attribute_handler);
            
            //! remove an handler associated to ans attirbute of a key
            bool removeKeyAttributeHandlerForHealt(const std::string& key_to_monitor,
                                                   int quantum_multiplier,
                                                   monitor_system::AbstractQuantumKeyAttributeHandler *attribute_handler);
            
            bool removeKeyAttributeHandlerForDataset(const std::string& key_to_monitor,
                                                     const unsigned int dataset_type,
                                                     int quantum_multiplier,
                                                     monitor_system::AbstractQuantumKeyAttributeHandler *attribute_handler);
            
            //! get the corresponding dataset key for a dataset type
            /*!
             \param node_uid, is the uniqu eid of the server
             \param dataset_type is the type of the dataset
             */
            std::string getDatasetKeyFromGeneralKey(const std::string& node_uid, const unsigned int dataset_type);
            
            //! get the corresponding healt key for node uid
            std::string getHealtKeyFromGeneralKey(const std::string& node_uid);
            
            //!register an event handler
            void registerEventHandler(chaos::metadata_service_client::event::AbstractEventHandler *handler);
            
            //!deregister an event handler
            void deregisterEventHandler(chaos::metadata_service_client::event::AbstractEventHandler *handler);
            
            void getNewCUController(const std::string& cu_id, chaos::metadata_service_client::node_controller::CUController **cu_ctrl_handler);
            
            void deleteCUController(chaos::metadata_service_client::node_controller::CUController *cu_ctrl_ptr);

            chaos::common::io::IODataDriverShrdPtr getDataProxyChannelNewInstance();

        };
    }
}

#endif /* defined(__CHAOSFramework__MetadataServiceClient__) */
