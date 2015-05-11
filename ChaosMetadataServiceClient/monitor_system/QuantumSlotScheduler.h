/*
 *	QuantumScheduler.h
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
#ifndef __CHAOSFramework__QuantumScheduler__
#define __CHAOSFramework__QuantumScheduler__

#include <ChaosMetadataServiceClient/monitor_system/monitor_system_types.h>
namespace chaos {
    namespace metadata_service_client {
        namespace monitor_system {
            //! forward declaration
            class MonitorManager;
            
            class QuantumSlotScheduler {
                friend class MonitorManager;
                
                QuantumSlotScheduler()
                ~QuantumSlotScheduler();
            public:
                
                //! add a new quantum slot for key
                void addKey(const std::string& key_to_monitor,
                            int quantum_slot,
                            QuantumSlotConsumer *consumer);
                
                void removeKey(const std::string& key_to_monitor,
                               int quantum_slot,
                               QuantumSlotConsumer *consumer);
            };
            
        }
    }
}

#endif /* defined(__CHAOSFramework__QuantumScheduler__) */
