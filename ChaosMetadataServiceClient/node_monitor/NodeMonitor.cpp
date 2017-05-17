/*
 *	NodeMonitor.cpp
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

#include <ChaosMetadataServiceClient/node_monitor/NodeMonitor.h>
#include <ChaosMetadataServiceClient/node_monitor/ControlUnitController.h>

#include <boost/format.hpp>

using namespace chaos::metadata_service_client::node_monitor;

NodeMonitor::NodeMonitor(chaos::metadata_service_client::monitor_system::MonitorManager *_monitor_manager,
                         chaos::metadata_service_client::api_proxy::ApiProxyManager *_api_proxy_manager,
                         ClientSetting *_setting):
monitor_manager(_monitor_manager),
api_proxy_manager(_api_proxy_manager),
setting(_setting) {}

NodeMonitor::~NodeMonitor() {
    map_fetcher.clear();
}

void NodeMonitor::init(void *init_data) throw (chaos::CException) {
    
}

void NodeMonitor::deinit() throw (chaos::CException) {
}

bool NodeMonitor::addHandlerToNodeMonitor(const std::string& node_uid,
                                          ControllerType controller_type,
                                          NodeMonitorHandler *handler_to_add) {
    CHAOS_ASSERT(monitor_manager);
    
    boost::unique_lock<boost::mutex> wl(map_fetcher_mutex);
    ChaosSharedPtr<NodeFetcher> fetcher;

    //check if we have the controller installed
    if(map_fetcher.count(node_uid) == 0) {
        //create new fetcher
        fetcher.reset(new NodeFetcher(monitor_manager,
                                      node_uid));
        
        //add fetcher to map
        map_fetcher.insert(make_pair(node_uid,
                                     fetcher));
    } else {
        fetcher = map_fetcher[node_uid];
    }
    return fetcher->addHanlderForControllerType(controller_type, handler_to_add);
}

bool NodeMonitor::removeHandlerToNodeMonitor(const std::string& node_uid,
                                             ControllerType controller_type,
                                             NodeMonitorHandler *handler_to_remove) {
    CHAOS_ASSERT(monitor_manager);
    boost::unique_lock<boost::mutex> wl(map_fetcher_mutex);

    //check if we have the controller installed
    if(map_fetcher.count(node_uid) == 0) return false;
    
    //remove handler from fetcher
    return map_fetcher[node_uid]->removeHandler(controller_type,
                                                handler_to_remove);
}
