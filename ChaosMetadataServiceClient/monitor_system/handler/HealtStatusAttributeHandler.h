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

#ifndef __CHAOSFramework__HealtStatusAttributeHandler__
#define __CHAOSFramework__HealtStatusAttributeHandler__

#include <chaos/common/chaos_constants.h>
#include <ChaosMetadataServiceClient/monitor_system/AbstractQuantumKeyAttributeHandler.h>

namespace chaos {
    namespace metadata_service_client {
        namespace monitor_system {
            namespace handler {
                
                //! Precreated handler for node healt status
                class HealtStatusAttributeHandler:
                public QuantumKeyAttributeStringHandler {
                public:
                    HealtStatusAttributeHandler(bool event_on_value_change = false):
                    QuantumKeyAttributeStringHandler(std::string(chaos::NodeHealtDefinitionKey::NODE_HEALT_STATUS), event_on_value_change){}
                };

                
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__HealtStatusAttributeHandler__) */
