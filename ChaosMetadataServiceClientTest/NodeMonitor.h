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


#ifndef CHAOS_NODEMONITOR_H
#define CHAOS_NODEMONITOR_H

#include <string>
#include <chaos_metadata_service_client/ChaosMetadataServiceClient.h>

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
