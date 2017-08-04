/*
 *	ConnectionManager.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 02/08/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__37CA350_9DAE_4B95_A1C3_B5FC41005DDE_ConnectionManager_h
#define __CHAOSFramework__37CA350_9DAE_4B95_A1C3_B5FC41005DDE_ConnectionManager_h

#include <string>
#include <chaos_micro_unit_toolkit/connection/connection_type.h>

namespace chaos {
    namespace micro_unit_toolkit {
        namespace connection {
            
            //! Entry point for a connection to chaos external unit server
            class ConnectionManager {
                
            public:
                ConnectionManager(const std::string& endpoint,
                           const ProtocolType protocol_type);
                ~ConnectionManager();
                
                
            };
        }
    }
}

#endif /* __CHAOSFramework__37CA350_9DAE_4B95_A1C3_B5FC41005DDE_ConnectionManager_h */
