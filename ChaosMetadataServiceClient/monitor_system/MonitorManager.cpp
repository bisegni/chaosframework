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
                                    int consumer_priority) {
    slot_scheduler->addKeyConsumer(key_to_monitor,
                                   quantum_multiplier,
                                   consumer,
                                   consumer_priority);
}

void MonitorManager::removeKeyConsumer(const std::string& key_to_monitor,
                                       int quantum_multiplier,
                                       QuantumSlotConsumer *consumer) {
    slot_scheduler->removeKeyConsumer(key_to_monitor,
                                      quantum_multiplier,
                                      consumer);
}