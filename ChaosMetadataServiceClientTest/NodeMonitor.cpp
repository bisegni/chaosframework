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

#include "NodeMonitor.h"
using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::monitor_system;

NodeMonitor::NodeMonitor(const std::string &node_id,
						 const uint32_t monitor_duration,
						 const uint32_t monitor_quantum_slot):
	node_id(node_id),
	monitor_duration(monitor_duration),
	monitor_quantum_slot(monitor_quantum_slot){}


NodeMonitor::~NodeMonitor(){}

void NodeMonitor::monitor_node(){
  //register this class with monitor system has handler
  ChaosMetadataServiceClient::getInstance()->addKeyConsumer(node_id,
															monitor_quantum_slot,
															this);

  //wait the desiderate amoun tof time
  sleep(monitor_duration);

  //remove this class as handler from the monitor system
  ChaosMetadataServiceClient::getInstance()->removeKeyConsumer(node_id,
															   monitor_quantum_slot,
															   this);
}

void NodeMonitor::quantumSlotHasNoData(const std::string &key){
  std::cout << key << " - " << "No Data" << std::endl;
}


void NodeMonitor::quantumSlotHasData(const std::string &key,
									 const KeyValue &value){
  std::cout << key << " - " << value->getJSONString() << "\n" << std::flush;
}