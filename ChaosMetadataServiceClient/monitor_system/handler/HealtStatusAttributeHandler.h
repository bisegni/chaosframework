//
//  HealtStatusAttributeHandler.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 14/05/15.
//  Copyright (c) 2015 INFN. All rights reserved.
//

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
                    HealtStatusAttributeHandler():
                    QuantumKeyAttributeStringHandler(std::string(chaos::NodeHealtDefinitionKey::NODE_HEALT_STATUS)){}
                };

                
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__HealtStatusAttributeHandler__) */
