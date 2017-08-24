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

#include "NodeMonitor.h"
using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::monitor_system;

NodeMonitor::NodeMonitor(const std::string &node_id,
						 const uint32_t monitor_duration,
						 const uint32_t monitor_quantum_slot):
	node_id(node_id),
	monitor_duration(monitor_duration),
	monitor_quantum_slot(monitor_quantum_slot){
    }


NodeMonitor::~NodeMonitor(){
    }

void NodeMonitor::registerConsumer() {
    //register this class with monitor system has handler
    ChaosMetadataServiceClient::getInstance()->addKeyConsumer(node_id,
                                                              monitor_quantum_slot,
                                                              this);

}

bool NodeMonitor::deregisterConsumer() {
    //remove this class as handler from the monitor system
    return ChaosMetadataServiceClient::getInstance()->removeKeyConsumer(node_id,
                                                                 monitor_quantum_slot,
                                                                 this,
                                                                 false);

}

void NodeMonitor::waitForPurge() {
    QuantumSlotConsumer::waitForCompletion();
}

void NodeMonitor::monitor_node(){

    registerConsumer();
    //wait the desiderate amount tof time
    sleep(monitor_duration);
    if(!deregisterConsumer()){
        waitForPurge();
    }
}

void NodeMonitor::quantumSlotHasNoData(const std::string &key){
  //std::cout << key << " - " << "No Data" << std::endl;
}


void NodeMonitor::quantumSlotHasData(const std::string &key,
									 const KeyValue &value){
  //std::cout << key << " - " << value->getJSONString() << "\n" << std::flush;
}
