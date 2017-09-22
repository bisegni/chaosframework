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
