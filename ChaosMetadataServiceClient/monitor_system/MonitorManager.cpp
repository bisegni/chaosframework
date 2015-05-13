/*
 *	MonitorManager.cpp
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

#include <ChaosMetadataServiceClient/monitor_system/MonitorManager.h>

using namespace chaos::metadata_service_client::monitor_system;

#define MM_INFO   INFO_LOG(MonitorManager)
#define MM_DBG    INFO_LOG(MonitorManager)
#define MM_ERR    INFO_LOG(MonitorManager)

MonitorManager::MonitorManager(chaos::common::network::NetworkBroker *_network_broker,
                               ClientSetting *_setting):
setting(_setting),
network_broker(_network_broker){
    
}

MonitorManager::~MonitorManager() {
    
}

void MonitorManager::init(void *init_data) throw (chaos::CException) {
    CHAOS_LASSERT_EXCEPTION(network_broker, MM_ERR, -1, "No network broker instance found")
    
    slot_scheduler = new QuantumSlotScheduler(network_broker);
    CHAOS_LASSERT_EXCEPTION(slot_scheduler, MM_ERR, -2, "Error allcaoting quantum slot scheduler")
    
    StartableService::initImplementation(slot_scheduler, init_data, "QuantumSlotScheduler", __PRETTY_FUNCTION__);
}

void MonitorManager::start() throw (chaos::CException) {
    StartableService::startImplementation(slot_scheduler, "QuantumSlotScheduler", __PRETTY_FUNCTION__);
}

void MonitorManager::stop() throw (chaos::CException) {
    StartableService::stopImplementation(slot_scheduler, "QuantumSlotScheduler", __PRETTY_FUNCTION__);
}

void MonitorManager::deinit() throw (chaos::CException) {
    StartableService::deinitImplementation(slot_scheduler, "QuantumSlotScheduler", __PRETTY_FUNCTION__);
}

void MonitorManager::resetEndpointList(std::vector<std::string> new_server_list) {
    slot_scheduler->setDataDriverEndpoints(new_server_list);
}

void MonitorManager::addKeyConsumer(const std::string& key_to_monitor,
                                    int quantum_multiplier,
                                    QuantumSlotConsumer *consumer,
                                    unsigned int consumer_priority) {
    //ad consumer to the scheduler
    slot_scheduler->addKeyConsumer(key_to_monitor,
                                   quantum_multiplier,
                                   consumer,
                                   consumer_priority);
}

//! add a new handler for a single attribute for a key
void MonitorManager::addKeyAttributeHandler(const std::string& key_to_monitor,
                                            int quantum_multiplier,
                                            AbstractQuantumKeyAttributeHandler *attribute_handler,
                                            unsigned int consumer_priority) {
    if(attribute_handler == NULL) return;
    boost::unique_lock<boost::mutex>   lock(map_mutex);
    
    QuantumKeyConsumer *consumer = NULL;
    
    //create unique key
    std::string unique_slot_key = CHAOS_QSS_COMPOSE_QUANTUM_SLOT_KEY(key_to_monitor, quantum_multiplier);
    
    //chec if we already have the consumer allocated
    if(map_quantum_key_consumer.count(unique_slot_key)) {
        //we already have the key consumer
        consumer = map_quantum_key_consumer[unique_slot_key];
    }else {
        //we need to allocate the key consumer
        map_quantum_key_consumer.insert(make_pair(unique_slot_key, (consumer = new QuantumKeyConsumer(key_to_monitor))));
        
        //add consuemr to the scheduler
        addKeyConsumer(key_to_monitor,
                       quantum_multiplier,
                       consumer);
    }
    
    consumer->addAttributeHandler(attribute_handler);
}

void MonitorManager::removeKeyConsumer(const std::string& key_to_monitor,
                                       int quantum_multiplier,
                                       QuantumSlotConsumer *consumer) {
    slot_scheduler->removeKeyConsumer(key_to_monitor,
                                      quantum_multiplier,
                                      consumer);
}

//! remove an handler associated to ans attirbute of a key
void MonitorManager::removeKeyAttributeHandler(const std::string& key_to_monitor,
                                               int quantum_multiplier,
                                               AbstractQuantumKeyAttributeHandler *attribute_handler) {
    if(attribute_handler == NULL) return;
    boost::unique_lock<boost::mutex>   lock(map_mutex);
    
    //create unique key
    std::string unique_slot_key = CHAOS_QSS_COMPOSE_QUANTUM_SLOT_KEY(key_to_monitor, quantum_multiplier);
    
    //we already have the key consumer
    QuantuKeyConsumerMapIterator it = map_quantum_key_consumer.find(unique_slot_key);
    if(it == map_quantum_key_consumer.end()) return;
    
    //remove the attribute within the consumer
    it->second->removeAttributeHandler(attribute_handler);
    
    //check if the attribute is the last, in this case we eed to remove all the consumer
    if(it->second->size() == 0) {
        //add consuemr to the scheduler
        removeKeyConsumer(key_to_monitor,
                          quantum_multiplier,
                          it->second);
 
        
        map_quantum_key_consumer.erase(it);
        //remove the consumer
        delete(it->second);
    }
}