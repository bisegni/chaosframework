/*
 * Copyright 2012, 12/10/2017 INFN
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

#ifndef chaos_cu_toolkit_driver_manager_driver_AbstractClientRemoteIODriver_h
#define chaos_cu_toolkit_driver_manager_driver_AbstractClientRemoteIODriver_h


//as client we need to send authentication message
#include <chaos/cu_toolkit/driver_manager/driver/AbstractRemoteIODriver.h>

#include <chaos/common/external_unit/ExternalUnitClientEndpoint.h>
namespace chaos {
    namespace cu {
        namespace driver_manager {
            namespace driver {
                typedef AbstractRemoteIODriver<common::external_unit::ExternalUnitClientEndpoint> ClientARIODriver;
                
                class AbstractClientRemoteIODriver:
                public ClientARIODriver {
                protected:
                    void driverInit(const char *initParameter);
                    void driverInit(const chaos::common::data::CDataWrapper& init_parameter);
                    void driverDeinit();
                    void handleNewConnection(const std::string& connection_identifier);
                public:
                    AbstractClientRemoteIODriver():
                    ClientARIODriver() {}
                    ~AbstractClientRemoteIODriver(){}
                };
            }
        }
    }
}
#endif /* chaos_cu_toolkit_driver_manager_driver_AbstractClientRemoteIODriver_h */
