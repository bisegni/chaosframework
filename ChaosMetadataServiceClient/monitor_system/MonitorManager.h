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

#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/utility/InizializableService.h>

namespace chaos {
    namespace metadata_service_client {
        //! forward declaration
        class ChaosMetadataServiceClient;
        
        namespace monitor_system {
            
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
                                    int consumer_priority = 500);
                
                //! remove a consumer by key and quantum
                void removeKeyConsumer(const std::string& key_to_monitor,
                                       int quantum_multiplier,
                                       QuantumSlotConsumer *consumer);
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__MonitorManager__) */
