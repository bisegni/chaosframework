/*
 *	HealtHeartBeatAttributeHandler.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
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
#ifndef CHAOSFramework_HealtHeartbeatAttributeHandler_h
#define CHAOSFramework_HealtHeartbeatAttributeHandler_h

#include <chaos/common/chaos_constants.h>
#include <ChaosMetadataServiceClient/monitor_system/AbstractQuantumKeyAttributeHandler.h>

namespace chaos {
    namespace metadata_service_client {
        namespace monitor_system {
            namespace handler {
                
                //! Precreated handler for node healt heartbeat
                class HealtHeartBeatAttributeHandler:
                public QuantumKeyAttributeInt64Handler {
                public:
                    HealtHeartBeatAttributeHandler():
                    QuantumKeyAttributeInt64Handler(std::string(chaos::NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP)){}
                };
                
                
            }
        }
    }
}

#endif
