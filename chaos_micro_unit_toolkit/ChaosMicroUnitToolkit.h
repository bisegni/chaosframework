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
            
            ChaosUniquePtr<connection::connection_adapter::AbstractConnectionAdapter> getConnectionAdapter(connection::ConnectionType protocol_type,
                                                                                                       const std::string& protocol_endpoint,
                                                                                                       const std::string& protocol_option);
            
            ChaosUniquePtr<connection::unit_proxy::raw_driver::RawDriverHandlerWrapper> createNewRawDriverHandlerWrapper(connection::ConnectionType protocol_type,
                                                                                                                         const std::string& protocol_endpoint,
                                                                                                                         const std::string& protocol_option,
                                                                                                                         connection::unit_proxy::UnitProxyHandler handler,
                                                                                                                         void *user_data,
                                                                                                                         const std::string& authorization_key);
        };
        
    }
}

#endif /* __CHAOSFramework__B9F7036_64EE_43D8_B174_A3D8B7AA7D78_ChaosMicroUnitToolkit_h */
