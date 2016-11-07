/*
 *	ControlUnitMonitor.h
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

#ifndef __CHAOSFramework__ControlUnitMonitor_h
#define __CHAOSFramework__ControlUnitMonitor_h

#include <ChaosMetadataServiceClient/node_monitor/NodeController.h>

namespace chaos {
    namespace metadata_service_client {
        namespace node_monitor {
            //! forward declaration
            class NodeMonitor;
            class NodeFetcher;
            
            class ControlUnitController:
            public NodeController {
                friend class NodeMonitor;
                friend class NodeFetcher;
                
                std::string cu_output_ds_key;
                //!last dataset received for helth data
                chaos::metadata_service_client::monitor_system::KeyValue last_ds_output;
                MapDatasetKeyValues                                       map_ds_out;
                
                std::string cu_input_ds_key;
                //!last dataset received for helth data
                chaos::metadata_service_client::monitor_system::KeyValue last_ds_input;
                MapDatasetKeyValues                                       map_ds_in;
                
                std::string cu_system_ds_key;
                //!last dataset received for helth data
                chaos::metadata_service_client::monitor_system::KeyValue last_ds_system;
                MapDatasetKeyValues                                       map_ds_sys;
                
                std::string cu_alarm_ds_key;
                //!last dataset received for helth data
                chaos::metadata_service_client::monitor_system::KeyValue last_ds_alarm;
                MapDatasetKeyValues                                       map_ds_alarm;
                
                void _updateDatsetKeyMapValue(chaos::metadata_service_client::monitor_system::KeyValue dataset,
                                              MapDatasetKeyValues& map);
                
                void _fireUpdateDSOnHandler(int dataset_type,
                                            MapDatasetKeyValues& map,
                                            bool changed);
                void _fireNoDSDataFoundOnHandler(int dataset_type);
            protected:
                ControlUnitController(const std::string& _node_uid);
                
                //!inherited method
                void quantumSlotHasData(const std::string& key,
                                        const monitor_system::KeyValue& value);
                //!inherited method
                void quantumSlotHasNoData(const std::string& key);
            public:
                virtual ~ControlUnitController();
                bool addHandler(NodeMonitorHandler *handler_to_add);
            };
        }
    }
}

#endif /* __CHAOSFramework__ControlUnitMonitor_h */
