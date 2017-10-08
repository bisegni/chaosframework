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

#ifndef __CHAOSFramework_E6EB27EC_C56A_4A03_BF1C_03373F140079_ExternalEchoEndpoint_h
#define __CHAOSFramework_E6EB27EC_C56A_4A03_BF1C_03373F140079_ExternalEchoEndpoint_h

#include <chaos/common/external_gateway/ExternalUnitEndpoint.h>

namespace chaos{
    namespace common {
        namespace external_gateway {
            
            class ExternalEchoEndpoint:
            public ExternalUnitEndpoint {
                unsigned int message_counter;
            protected:
                //!inherited method by @ExternalUnitEndpoint
                void handleNewConnection(const std::string& connection_identifier);
                //!inherited method by @ExternalUnitEndpoint
                void handleDisconnection(const std::string& connection_identifier);
                
                int handleReceivedeMessage(const std::string& connection_identifier,
                                           chaos::common::data::CDWUniquePtr message);
            public:
                ExternalEchoEndpoint();
                ~ExternalEchoEndpoint();
            };
            
        }
    }
}

#endif /* __CHAOSFramework_E6EB27EC_C56A_4A03_BF1C_03373F140079_ExternalEchoEndpoint_h */
