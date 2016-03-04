/*
 *	MonitorManager.h
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

#ifndef __CHAOSFramework__MonitorManager__
#define __CHAOSFramework__MonitorManager__

#include <ChaosMetadataServiceClient/metadata_service_client_types.h>
#include <ChaosMetadataServiceClient/monitor_system/monitor_system_types.h>
#include <ChaosMetadataServiceClient/monitor_system/QuantumSlotScheduler.h>
#include <ChaosMetadataServiceClient/monitor_system/QuantumKeyConsumer.h>

#include <chaos/common/chaos_types.h>
#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/async_central/async_central.h>
#include <chaos/common/utility/InizializableService.h>

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

#include <map>
#include <string>

namespace chaos {
    namespace metadata_service_client {
        //! forward declaration
        class ChaosMetadataServiceClient;
        
        namespace monitor_system {
            
            CHAOS_DEFINE_MAP_FOR_TYPE(std::string,
                                      QuantumKeyConsumer*,
                                      QuantuKeyConsumerMap);
            
            typedef std::queue< QuantumKeyConsumer* >  QuantuKeyConsumerToEraseQueue;
            
            /*!
             class that manage the monitoring of node healt and
             control unit attribute value
             */
            class MonitorManager:
            public chaos::common::utility::StartableService,
            protected chaos::common::async_central::TimerHandler {
                friend class chaos::common::utility::StartableServiceContainer<MonitorManager>;
                friend class chaos::metadata_service_client::ChaosMetadataServiceClient;
                
                //! Applciation settings
                ClientSetting *setting;
                
                //! network broker service
                chaos::common::network::NetworkBroker *network_broker;
                
                //! queue and mutex used for key consumer to be purged
                boost::mutex                    mutex_queue_to_purge;
                QuantuKeyConsumerToEraseQueue   queue_to_purge;
                
                //!quantum slot scheduler
                QuantumSlotScheduler *slot_scheduler;
                
                //!map for QuantumKeyAttributeHandler structure
                boost::mutex            map_mutex;
                QuantuKeyConsumerMap    map_quantum_key_consumer;
                
                MonitorManager(chaos::common::network::NetworkBroker *_network_broker,
                               ClientSetting *_setting);
                ~MonitorManager();
            public:
                
                void init(void *init_data) throw (chaos::CException);
                void start() throw (chaos::CException);
                void stop() throw (chaos::CException);
                void deinit() throw (chaos::CException);
                
                void resetEndpointList(std::vector<std::string> new_server_list);
                
                //! add a new quantum slot for key
                void addKeyConsumer(const std::string& key_to_monitor,
                                    int quantum_multiplier,
                                    QuantumSlotConsumer *consumer,
                                    unsigned int consumer_priority = 500);
                
                //! add a new handler for a single attribute for a key
                void addKeyAttributeHandler(const std::string& key_to_monitor,
                                            int quantum_multiplier,
                                            AbstractQuantumKeyAttributeHandler *attribute_handler,
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
                                         QuantumSlotConsumer *consumer,
                                         bool wait_completion = true);
                
                //! remove an handler associated to ans attirbute of a key
                void removeKeyAttributeHandler(const std::string& key_to_monitor,
                                               int quantum_multiplier,
                                               AbstractQuantumKeyAttributeHandler *attribute_handler);
                
            protected:
                //! timer slot
                void timeout();
                void purgeKeyConsumer(bool all);
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__MonitorManager__) */
