/*
 *	NodeFetcher.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 07/04/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__NodeFetcher_h
#define __CHAOSFramework__NodeFetcher_h
#include <chaos/common/chaos_types.h>

#include <ChaosMetadataServiceClient/node_monitor/node_monitor.h>
#include <ChaosMetadataServiceClient/monitor_system/monitor_system.h>

#include <boost/thread.hpp>

#include <map>
#include <string>

namespace chaos {
    namespace metadata_service_client {
        
        namespace node_monitor {
            //! forward declaration
            class NodeMonitor;
            
            //associate a type to a controlelr instance
            CHAOS_DEFINE_MAP_FOR_TYPE(ControllerType, ChaosSharedPtr<node_monitor::NodeController>, NodeTypeControllerMap)
            
            class NodeFetcher:
            public chaos::metadata_service_client::monitor_system::QuantumSlotConsumer {
                friend class NodeMonitor;
                
                const std::string             node_uid;
                //keep track of how many times a key is needed and rmeoved when no other instance arre needed
                std::map<std::string, int>    map_monitor_key_registered_times;
                
                boost::mutex            maps_mutex;
                NodeTypeControllerMap   map_type_controller;
                
                //reference to global monitor manager instance
                chaos::metadata_service_client::monitor_system::MonitorManager *monitor_manager;
                
                //---private method---
                ChaosSharedPtr<node_monitor::NodeController> getControllerForType(ControllerType type);
                
                bool removeHandlerFromController(ControllerType type,
                                                 NodeMonitorHandler *handler);
                
                void managmentKeyToMonitor(const MonitorKeyList& key_to_listen, bool monitor);
            protected:
                NodeFetcher(chaos::metadata_service_client::monitor_system::MonitorManager *_monitor_manager,
                            const std::string& _node_uid);
                //!inherited method
                void quantumSlotHasData(const std::string& key,
                                        const chaos::metadata_service_client::monitor_system::KeyValue& value);
                //!inherited method
                void quantumSlotHasNoData(const std::string& key);
                
            public:
                ~NodeFetcher();
                bool addHanlderForControllerType(ControllerType type, NodeMonitorHandler *handler);
                bool removeHandler(ControllerType type, NodeMonitorHandler *handler);
                
            };
        }
    }
}

#endif /* __CHAOSFramework__NodeFetcher_h */
