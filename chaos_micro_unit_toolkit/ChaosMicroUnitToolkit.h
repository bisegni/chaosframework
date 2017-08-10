/*
 *	ChaosMicroUnitToolkit.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 01/08/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__B9F7036_64EE_43D8_B174_A3D8B7AA7D78_ChaosMicroUnitToolkit_h
#define __CHAOSFramework__B9F7036_64EE_43D8_B174_A3D8B7AA7D78_ChaosMicroUnitToolkit_h

#include <chaos_micro_unit_toolkit/micro_unit_toolkit_types.h>
#include <chaos_micro_unit_toolkit/connection/connection.h>

namespace chaos {
    namespace micro_unit_toolkit {
        
        //! main singleton lab entrypoint
        class ChaosMicroUnitToolkit {
            connection::ConnectionManager cman;
        public:
            ChaosMicroUnitToolkit();
            ~ChaosMicroUnitToolkit();
            
            ChaosUniquePtr<connection::protocol_adapter::AbstractProtocolAdapter> getConnectionAdapter(connection::ProtocolType protocol_type,
                                                                                                       const std::string& protocol_endpoint,
                                                                                                       const std::string& protocol_option);
            
            ChaosUniquePtr<connection::unit_proxy::raw_driver::RawDriverHandlerWrapper> createNewRawDriverHandlerWrapper(connection::ProtocolType protocol_type,
                                                                                                                         const std::string& protocol_endpoint,
                                                                                                                         const std::string& protocol_option,
                                                                                                                         connection::unit_proxy::UnitProxyHandler handler,
                                                                                                                         void *user_data,
                                                                                                                         const std::string& authorization_key);
        };
        
    }
}

#endif /* __CHAOSFramework__B9F7036_64EE_43D8_B174_A3D8B7AA7D78_ChaosMicroUnitToolkit_h */
