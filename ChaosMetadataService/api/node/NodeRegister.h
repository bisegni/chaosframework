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
                    //!perform te agent registration
                    chaos::common::data::CDataWrapper *agentRegistration(chaos::common::data::CDataWrapper *api_data,
                                                                         bool& detach_data) throw(chaos::CException);
                    //! perform registration for specific unit server
                    chaos::common::data::CDataWrapper *unitServerRegistration(chaos::common::data::CDataWrapper *api_data,
                                                                              bool& detach_data) throw(chaos::CException);
                    
                    //! perform specific registration for control unit
                    chaos::common::data::CDataWrapper *controlUnitRegistration(chaos::common::data::CDataWrapper *api_data,
                                                                               bool& detach_data) throw(chaos::CException);
                public:
                    NodeRegister();
                    ~NodeRegister();
                    chaos::common::data::CDataWrapper *execute(chaos::common::data::CDataWrapper *api_data,
                                                               bool& detach_data) throw(chaos::CException);
                };
                
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__UnitServerRegisterApi__) */
