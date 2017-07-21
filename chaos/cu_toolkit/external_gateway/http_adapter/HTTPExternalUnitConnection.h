/*
 *	HTTPExternalConnection.h
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

#ifndef __CHAOSFramework_B4F38257_89FD_45DB_8E0C_2900506EFFAC_HTTPExternalConnection_h
#define __CHAOSFramework_B4F38257_89FD_45DB_8E0C_2900506EFFAC_HTTPExternalConnection_h

#include <chaos/cu_toolkit/external_gateway/ExternalUnitConnection.h>
#include <chaos/cu_toolkit/external_gateway/http_adapter/http_adapter_types.h>
#include <chaos/cu_toolkit/additional_lib/mongoose.h>

namespace chaos{
    namespace cu {
        namespace external_gateway {
            namespace http_adapter{
                class HTTPAdapter;
                
                class HTTPExternalUnitConnection:
                public ExternalUnitConnection {
                    friend class HTTPAdapter;
                    //!associated websocket connection
                    mg_connection *nc = NULL;
                    //!manage all data received by the associated websocket
                    int handleWSIncomingData(const ChaosUniquePtr<WorkRequest>& request);
                    
                    int sendDataToConnection(const chaos::common::data::CDBufferUniquePtr data,
                                             const EUCMessageOpcode opcode);
                public:
                    HTTPExternalUnitConnection(mg_connection *_nc,
                                               ExternalUnitEndpoint *_endpoint,
                                               ChaosUniquePtr<chaos::cu::external_gateway::serialization::AbstractExternalSerialization> _serializer_adaptor);
                    ~HTTPExternalUnitConnection();
                    
                    void closeConnection();
                };
            }
        }
    }
}

#endif /* __CHAOSFramework_B4F38257_89FD_45DB_8E0C_2900506EFFAC_HTTPExternalConnection_h */
