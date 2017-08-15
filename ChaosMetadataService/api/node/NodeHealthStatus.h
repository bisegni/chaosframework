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

#ifndef __CHAOSFramework_EC4E1122_FF1C_40DB_B792_2080B18E6F06_NodeHealthStatus_h
#define __CHAOSFramework_EC4E1122_FF1C_40DB_B792_2080B18E6F06_NodeHealthStatus_h

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
                class NodeHealthStatus:
                public AbstractApi {
                public:
                    NodeHealthStatus();
                    ~NodeHealthStatus();
                    chaos::common::data::CDataWrapper *execute(chaos::common::data::CDataWrapper *api_data,
                                                               bool& detach_data) throw(chaos::CException);
                };
                
            }
        }
    }
}

#endif /* __CHAOSFramework_EC4E1122_FF1C_40DB_B792_2080B18E6F06_NodeHealthStatus_h */
