/*
 *	NodeMonitorHandler.cpp
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

#include <ChaosMetadataServiceClient/node_monitor/NodeMonitorHandler.h>

using namespace chaos::metadata_service_client::node_monitor;



NodeMonitorHandler::NodeMonitorHandler():
handler_uuid(chaos::common::utility::UUIDUtil::generateUUIDLite()){}

NodeMonitorHandler::~NodeMonitorHandler() {}

void NodeMonitorHandler::nodeChangedOnlineState(const std::string& node_uid,
                                                OnlineState old_status,
                                                OnlineState new_status) {
    
}

void NodeMonitorHandler::nodeChangedInternalState(const std::string& node_uid,
                                                  const std::string& old_status,
                                                  const std::string& new_status) {
    
}

void NodeMonitorHandler::nodeChangedProcessResource(const std::string& node_uid,
                                                    const ProcessResource& old_proc_res,
                                                    const ProcessResource& new_proc_res) {
    
}

void NodeMonitorHandler::nodeChangedErrorInformation(const std::string& node_uid,
                                                     const ErrorInformation& old_status,
                                                     const ErrorInformation& new_status) {
    
}

void NodeMonitorHandler::handlerHasBeenRegistered(const std::string& node_uid,
                                                  const HealthInformation& current_health_status) {
    
}