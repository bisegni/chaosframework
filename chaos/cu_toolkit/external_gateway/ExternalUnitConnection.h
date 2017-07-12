/*
 *	ExternalConnection.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 11/07/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_A9E1864B_5C02_4ECA_8289_AFA41CE09C52_ExternalConnection_h
#define __CHAOSFramework_A9E1864B_5C02_4ECA_8289_AFA41CE09C52_ExternalConnection_h

#include <chaos/common/chaos_types.h>

namespace chaos {
    namespace cu {
        namespace external_gateway {
            class AbstractAdapter;
            class ExternalUnitEndpoint;
            
            //! Identify an external connection
            class ExternalUnitConnection {
            protected:
                //!endpoint that own the connection
                ExternalUnitEndpoint *endpoint;
            protected:
                int sendDataToEndpoint(const std::string& data);
            public:
                //! end point identifier
                const std::string connection_identifier;
                
                ExternalUnitConnection(ExternalUnitEndpoint *_endpoint);
                virtual ~ExternalUnitConnection();
                
                virtual int sendData(const std::string& data) = 0;
            };
        }
    }
}

#endif /* __CHAOSFramework_A9E1864B_5C02_4ECA_8289_AFA41CE09C52_ExternalConnection_h */
