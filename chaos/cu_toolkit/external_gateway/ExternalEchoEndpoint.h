/*
 *	ExternalEchoEndpoint.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 12/07/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_E6EB27EC_C56A_4A03_BF1C_03373F140079_ExternalEchoEndpoint_h
#define __CHAOSFramework_E6EB27EC_C56A_4A03_BF1C_03373F140079_ExternalEchoEndpoint_h

#include <chaos/cu_toolkit/external_gateway/ExternalUnitEndpoint.h>

namespace chaos{
    namespace cu {
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
