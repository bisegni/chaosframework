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
#ifndef __CHAOSFramework__UnitServerRegisterApi__
#define __CHAOSFramework__UnitServerRegisterApi__

#include "../AbstractApi.h"

namespace chaos {
    namespace metadata_service {
        namespace api {
            namespace node {
                
                //! Implement the api for the node registration
                /*!
                 The received pack is scanned to find the unique node id and the his type.
                 If the received node is recognized as belonging to a default type, it is
                 forwarded to the method that performs the specific registration for every type.
                 Otherwhise the datapack will be saved in node structure without further
                 considerations.
                 */
                class NodeRegister:
                public AbstractApi {
                    chaos::common::data::CDWUniquePtr unitServerRegistration(chaos::common::data::CDWUniquePtr api_data);
                    chaos::common::data::CDWUniquePtr agentRegistration(chaos::common::data::CDWUniquePtr api_data);
                    chaos::common::data::CDWUniquePtr simpleRegistration(chaos::common::data::CDWUniquePtr api_data);

                    chaos::common::data::CDWUniquePtr controlUnitRegistration(chaos::common::data::CDWUniquePtr api_data);
                public:
                    NodeRegister();
                    ~NodeRegister();
                    chaos::common::data::CDWUniquePtr execute(chaos::common::data::CDWUniquePtr api_data);
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__UnitServerRegisterApi__) */
