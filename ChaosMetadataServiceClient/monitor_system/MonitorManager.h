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

#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/utility/InizializableService.h>

#include <map>
#include <string>

namespace chaos {
    namespace metadata_service_client {
        //! forward declaration
        class ChaosMetadataServiceClient;
        
        namespace monitor_system {
            
            typedef std::map<std::string, QuantumKeyConsumer*>           QuantuKeyConsumerMap;
            typedef std::map<std::string, QuantumKeyConsumer*>::iterator QuantuKeyConsumerMapIterator;
            /*!
             class that manage the monitoring of node healt and
             control unit attribute value
             */
            class MonitorManager:
            public chaos::common::utility::StartableService {
                friend class chaos::common::utility::StartableServiceContainer<MonitorManager>;
                friend class chaos::metadata_service_client::ChaosMetadataServiceClient;
                
                //! Applciation settings
                ClientSetting *setting;
                
                //! network broker service
                chaos::common::network::NetworkBroker *network_broker;
                
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
                
                //! remove a consumer by key and quantum
                void removeKeyConsumer(const std::string& key_to_monitor,
                                       int quantum_multiplier,
                                       QuantumSlotConsumer *consumer);
                
                //! remove an handler associated to ans attirbute of a key
                void removeKeyAttributeHandler(const std::string& key_to_monitor,
                                               int quantum_multiplier,
                                               AbstractQuantumKeyAttributeHandler *attribute_handler);
                
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__MonitorManager__) */
