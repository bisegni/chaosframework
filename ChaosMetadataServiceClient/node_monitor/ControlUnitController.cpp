/*
 *	ControlUnitMonitor.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 16/03/16 INFN, National Institute of Nuclear Physics
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

#include <ChaosMetadataServiceClient/node_monitor/ControlUnitController.h>

using namespace chaos::metadata_service_client::node_monitor;

ControlUnitController::ControlUnitController(const std::string& _node_uid):
NodeController(_node_uid){
    //add common node dataset
    monitor_key_list.push_back(getNodeUID()+chaos::DataPackPrefixID::OUTPUT_DATASE_PREFIX);
    //add common node dataset
    monitor_key_list.push_back(getNodeUID()+chaos::DataPackPrefixID::INPUT_DATASE_PREFIX);
    //add common node dataset
    monitor_key_list.push_back(getNodeUID()+chaos::DataPackPrefixID::SYSTEM_DATASE_PREFIX);
}

ControlUnitController::~ControlUnitController() {}

void ControlUnitController::quantumSlotHasData(const std::string& key,
                                               const monitor_system::KeyValue& value) {
    NodeController::quantumSlotHasData(key,
                                       value);
}

void ControlUnitController::quantumSlotHasNoData(const std::string& key) {
    NodeController::quantumSlotHasNoData(key);
}