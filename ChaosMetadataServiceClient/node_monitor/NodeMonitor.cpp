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

#define MAKE_NODE_UID_CONTROLLER_TYPE_KEY(nuid, ctype)\
boost::str(boost::format("%1%_%2%")%nuid%ctype)

using namespace chaos::metadata_service_client::node_monitor;

NodeMonitor::NodeMonitor(chaos::metadata_service_client::monitor_system::MonitorManager *_monitor_manager,
                         ClientSetting *_setting):
monitor_manager(_monitor_manager),
setting(_setting) {}

NodeMonitor::~NodeMonitor() {}

void NodeMonitor::init(void *init_data) throw (chaos::CException) {
    
}

void NodeMonitor::deinit() throw (chaos::CException) {
    for(NodeControllerMapIterator it = map_monitor_controller.begin(), it_end = map_monitor_controller.end();
        it != it_end;
        it++){
        boost::shared_ptr<NodeController> controller = it->second;
        const MonitorKeyList& key_to_register = controller->getMonitorKeyList();
        
        for(MonitorKeyListConstIterator it = key_to_register.begin(),
            it_end = key_to_register.end();
            it != it_end;
            it++) {
            monitor_manager->removeKeyConsumer(*it, 10, controller.get());
        }

    }
    map_monitor_controller.clear();
}

void NodeMonitor::startNodeMonitor(const std::string& node_uid,
                                   ControllerType controller_type) {
    //check if we already have the controller installed
    boost::shared_ptr<NodeController> new_controller;
    const std::string composed_key = MAKE_NODE_UID_CONTROLLER_TYPE_KEY(node_uid, controller_type);
    if(map_monitor_controller.count(composed_key)) return;
    
    switch(controller_type){
        case ControllerTypeNode:{
            new_controller.reset(new NodeController(node_uid));
            break;
        }
            
        case ControllerTypeNodeControlUnit:{
            new_controller.reset(new ControlUnitController(node_uid));
            break;
        }
    }
   
    map_monitor_controller.insert(make_pair(composed_key,
                                            new_controller));
    const MonitorKeyList& key_to_register = new_controller->getMonitorKeyList();
    
    for(MonitorKeyListConstIterator it = key_to_register.begin(),
        it_end = key_to_register.end();
        it != it_end;
        it++) {
        monitor_manager->addKeyConsumer(*it, 10, new_controller.get());
    }
}

void NodeMonitor::stopNodeMonitor(const std::string& node_uid,
                                  ControllerType controller_type) {
    const std::string composed_key = MAKE_NODE_UID_CONTROLLER_TYPE_KEY(node_uid, controller_type);
    //check if we have the controller installed
    if(map_monitor_controller.count(composed_key) == 0) return;
    
    boost::shared_ptr<NodeController> controller = map_monitor_controller[composed_key];
    const MonitorKeyList& key_to_register = controller->getMonitorKeyList();
    
    for(MonitorKeyListConstIterator it = key_to_register.begin(),
        it_end = key_to_register.end();
        it != it_end;
        it++) {
        monitor_manager->removeKeyConsumer(*it, 10, controller.get());
    }
    map_monitor_controller.erase(composed_key);
}

bool NodeMonitor::addHandlerToNodeMonitor(const std::string& node_uid,
                                          ControllerType controller_type,
                                          NodeMonitorHandler *handler_to_add) {
    CHAOS_ASSERT(monitor_manager);
    boost::unique_lock<boost::mutex> wl(map_monitor_controller_mutex);
    const std::string composed_key = MAKE_NODE_UID_CONTROLLER_TYPE_KEY(node_uid, controller_type);

    //check if we have the controller installed
    if(map_monitor_controller.count(composed_key) == 0) {
        startNodeMonitor(node_uid, controller_type);
    }
    return map_monitor_controller[composed_key]->addHandler(handler_to_add);
}

bool NodeMonitor::removeHandlerToNodeMonitor(const std::string& node_uid,
                                             ControllerType controller_type,
                                             NodeMonitorHandler *handler_to_remove) {
    CHAOS_ASSERT(monitor_manager);
    bool result = true;
    boost::unique_lock<boost::mutex> wl(map_monitor_controller_mutex);
    const std::string composed_key = MAKE_NODE_UID_CONTROLLER_TYPE_KEY(node_uid, controller_type);

    //check if we have the controller installed
    if(map_monitor_controller.count(composed_key) == 0) return false;
    if((result = map_monitor_controller[node_uid]->removeHandler(handler_to_remove))) {
        //check the seize of the handler list in controller and
        //in case it is empty delete also the controller
        if(map_monitor_controller[node_uid]->getMonitorHandlerList().size() == 0){
            //delete the controller
            stopNodeMonitor(node_uid, controller_type);
        }
    }
    return result;
}