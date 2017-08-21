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

#ifndef __CHAOSFramework__ControlUnitMonitor_h
#define __CHAOSFramework__ControlUnitMonitor_h

#include <chaos_metadata_service_client/node_monitor/NodeController.h>

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
                
                std::string cu_warning_ds_key;
                //!last dataset received for helth data
                chaos::metadata_service_client::monitor_system::KeyValue last_ds_warning;
                MapDatasetKeyValues                                       map_ds_warning;
                
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
