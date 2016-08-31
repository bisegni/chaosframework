/*
 *	ControlUnitMonitorHandler.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 29/03/16 INFN, National Institute of Nuclear Physics
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


#include <ChaosMetadataServiceClient/node_monitor/ControlUnitMonitorHandler.h>

using namespace chaos::metadata_service_client::node_monitor;

ControlUnitMonitorHandler::ControlUnitMonitorHandler() {}

ControlUnitMonitorHandler::~ControlUnitMonitorHandler() {}

void ControlUnitMonitorHandler::updatedDS(const std::string& control_unit_uid,
                                          int dataset_type,
                                          MapDatasetKeyValues& dataset_key_values) {
    
}

void ControlUnitMonitorHandler::noDSDataFound(const std::string& control_unit_uid,
                                              int dataset_type) {
    
}