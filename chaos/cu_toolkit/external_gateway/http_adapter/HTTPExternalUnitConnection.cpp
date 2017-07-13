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

using namespace chaos::cu::external_gateway::http_adapter;

HTTPExternalUnitConnection::HTTPExternalUnitConnection(mg_connection *_nc,
                                                       ExternalUnitEndpoint *_endpoint):
ExternalUnitConnection(_endpoint),
nc(_nc){}
HTTPExternalUnitConnection::~HTTPExternalUnitConnection(){}

int HTTPExternalUnitConnection::handleWSIncomingData(const ChaosUniquePtr<WorkRequest>& request) {
    const std::string message(&request->buffer[0], request->buffer.size());
    return sendDataToEndpoint(message);
}

int HTTPExternalUnitConnection::sendData(const std::string& data,
                                         const EUCMessageOpcode opcode) {
    switch (opcode) {
        case EUCMessageOpcodeWhole:
            mg_send_websocket_frame(nc, WEBSOCKET_OP_TEXT, data.c_str(), data.size());
            break;
        case EUCPhaseStartFragment:
            mg_send_websocket_frame(nc, WEBSOCKET_OP_TEXT|WEBSOCKET_DONT_FIN, data.c_str(), data.size());
            break;
            
        case EUCPhaseContinueFragment:
            mg_send_websocket_frame(nc, WEBSOCKET_OP_TEXT|WEBSOCKET_DONT_FIN, data.c_str(), data.size());
            break;
            
        case EUCPhaseEndFragment:
            mg_send_websocket_frame(nc, WEBSOCKET_OP_TEXT, data.c_str(), data.size());
            break;
    }
    
    return 0;
}
