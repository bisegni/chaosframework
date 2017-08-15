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
