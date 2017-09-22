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

#ifndef __CHAOSFramework__ControlUnitMonitorHandler_h
#define __CHAOSFramework__ControlUnitMonitorHandler_h

#include <chaos_metadata_service_client/node_monitor/NodeMonitorHandler.h>

namespace chaos {
    namespace metadata_service_client {
        namespace node_monitor {
            
            class ControlUnitMonitorHandler:
            public NodeMonitorHandler {
                
            public:
                ControlUnitMonitorHandler();
                virtual ~ControlUnitMonitorHandler();
                
                virtual void updatedDS(const std::string& control_unit_uid,
                                       int dataset_type,
                                       MapDatasetKeyValues& dataset_key_values);
                
                virtual void noDSDataFound(const std::string& control_unit_uid,
                                           int dataset_type);
            };
            
        }
    }
}

#endif /* __CHAOSFramework__ControlUnitMonitorHandler_h */
