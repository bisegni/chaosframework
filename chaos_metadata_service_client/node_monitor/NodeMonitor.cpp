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

#include <chaos_metadata_service_client/node_monitor/NodeMonitor.h>
#include <chaos_metadata_service_client/node_monitor/ControlUnitController.h>
#include <chaos_metadata_service_client/ChaosMetadataServiceClient.h>

#include <boost/format.hpp>
using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::node_monitor;

NodeMonitor::NodeMonitor(chaos::metadata_service_client::monitor_system::MonitorManager *_monitor_manager,
                         chaos::metadata_service_client::api_proxy::ApiProxyManager *_api_proxy_manager):
monitor_manager(_monitor_manager),
api_proxy_manager(_api_proxy_manager){}

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
