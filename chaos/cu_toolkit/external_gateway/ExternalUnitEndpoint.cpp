/*
 *	ExternalEndpoint.cpp
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

#include <chaos/cu_toolkit/external_gateway/ExternalUnitEndpoint.h>

using namespace chaos::cu::external_gateway;

ExternalUnitEndpoint::ExternalUnitEndpoint(const std::string& _endpoint_identifier):
endpoint_identifier(_endpoint_identifier){
    
}

ExternalUnitEndpoint::~ExternalUnitEndpoint() {
    
}

int ExternalUnitEndpoint::sendMessage(const std::string& connection_identifier,
                                      ChaosUniquePtr<chaos::common::data::CDataWrapper> message,
                                      const EUCMessageOpcode opcode) {
    LMapConnectionReadLock rl = map_connection.getReadLockObject();
    if(map_connection().count(connection_identifier) == 0) return -1;
    //send data to the coneection
    return map_connection()[connection_identifier]->sendData(ChaosMoveOperator(message),
                                                             opcode);
}

const std::string& ExternalUnitEndpoint::getIdentifier() {
    return endpoint_identifier;
}

int ExternalUnitEndpoint::addConnection(ExternalUnitConnection& new_connection) {
    LMapConnectionWriteLock wl = map_connection.getWriteLockObject();
    map_connection().insert(MapConnectionPair(new_connection.connection_identifier, &new_connection));
    handleNewConnection(new_connection.connection_identifier);
    return 0;
}

int ExternalUnitEndpoint::removeConnection(ExternalUnitConnection& removed_connection) {
    LMapConnectionWriteLock wl = map_connection.getWriteLockObject();
    handleDisconnection(removed_connection.connection_identifier);
    map_connection().erase(removed_connection.connection_identifier);
    return 0;
}
