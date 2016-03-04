/*
 *	StartableService.h
 *	!CHAOS
 *	Created by Claudio Bisegni on 14/09/15.
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


#ifndef CHAOS_NODEMONITOR_H
#define CHAOS_NODEMONITOR_H

#include <string>
#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

class NodeMonitor :
public chaos::metadata_service_client::monitor_system::QuantumSlotConsumer {
    const std::string node_id;
    const uint32_t monitor_duration;
    const uint32_t monitor_quantum_slot;
    
    
    void quantumSlotHasData(const std::string &key,
                            const chaos::metadata_service_client::monitor_system::KeyValue &value);
    
    void quantumSlotHasNoData(const std::string &key);
    
public:
    
    NodeMonitor(const std::string &node_id,
                const uint32_t monitor_duration,
                const uint32_t monitor_quantum_slot);
    
    
    virtual ~NodeMonitor();
    
    void registerConsumer();
    
    bool deregisterConsumer();
    
    void waitForPurge();
    
    void monitor_node();
    
};


#endif //CHAOS_NODEMONITOR_H
