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
