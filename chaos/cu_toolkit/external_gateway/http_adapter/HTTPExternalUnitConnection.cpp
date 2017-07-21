/*
 *	HTTPExternalConnection.cpp
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

#include <chaos/cu_toolkit/external_gateway/http_adapter/HTTPExternalUnitConnection.h>

using namespace chaos::common::data;
using namespace chaos::cu::external_gateway::http_adapter;

HTTPExternalUnitConnection::HTTPExternalUnitConnection(mg_connection *_nc,
                                                       ExternalUnitEndpoint *_endpoint,
                                                       ChaosUniquePtr<chaos::cu::external_gateway::serialization::AbstractExternalSerialization> _serializer_adaptor):
ExternalUnitConnection(_endpoint, ChaosMoveOperator(_serializer_adaptor)),
nc(_nc){}
HTTPExternalUnitConnection::~HTTPExternalUnitConnection(){}

int HTTPExternalUnitConnection::handleWSIncomingData(const ChaosUniquePtr<WorkRequest>& request) {
    return sendDataToEndpoint(ChaosMoveOperator(request->buffer));
}

int HTTPExternalUnitConnection::sendDataToConnection(const chaos::common::data::CDBufferUniquePtr data,
                                                     const EUCMessageOpcode opcode) {
    switch (opcode) {
        case EUCMessageOpcodeWhole:
            mg_send_websocket_frame(nc, WEBSOCKET_OP_TEXT, data->getBuffer(), data->getBufferSize());
            break;
        case EUCPhaseStartFragment:
            mg_send_websocket_frame(nc, WEBSOCKET_OP_TEXT|WEBSOCKET_DONT_FIN, data->getBuffer(), data->getBufferSize());
            break;
            
        case EUCPhaseContinueFragment:
            mg_send_websocket_frame(nc, WEBSOCKET_OP_TEXT|WEBSOCKET_DONT_FIN, data->getBuffer(), data->getBufferSize());
            break;
            
        case EUCPhaseEndFragment:
            mg_send_websocket_frame(nc, WEBSOCKET_OP_TEXT, data->getBuffer(), data->getBufferSize());
            break;
    }
    
    return 0;
}

void HTTPExternalUnitConnection::closeConnection() {
    mg_send_websocket_frame(nc, WEBSOCKET_OP_CLOSE, NULL, 0);
}
