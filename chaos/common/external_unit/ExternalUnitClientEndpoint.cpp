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

#include <chaos/common/external_unit/ExternalUnitClientEndpoint.h>

using namespace chaos::common::data;
using namespace chaos::common::external_unit;

ExternalUnitClientEndpoint::ExternalUnitClientEndpoint() {}

ExternalUnitClientEndpoint::ExternalUnitClientEndpoint(const std::string& _client_identification):
client_identification(_client_identification){}

ExternalUnitClientEndpoint::~ExternalUnitClientEndpoint() {}

int ExternalUnitClientEndpoint::sendMessage(CDWUniquePtr message,
                                      const EUCMessageOpcode opcode) {
    LExternalUnitConnectionReadLock rl = current_connection.getReadLockObject();
    if(current_connection() == NULL) return -1;
    //send data to the coneection
    return current_connection()->sendData(ChaosMoveOperator(message),
                                                             opcode);
}

std::string ExternalUnitClientEndpoint::getIdentifier() {
    return client_identification;
}

int ExternalUnitClientEndpoint::setConnection(ExternalUnitConnection& new_connection) {
    LExternalUnitConnectionWriteLock rl = current_connection.getWriteLockObject();
    current_connection() = &new_connection;
    handleNewConnection(new_connection.connection_identifier);
    return 0;
}

int ExternalUnitClientEndpoint::removeConnection(ExternalUnitConnection& removed_connection) {
    LExternalUnitConnectionWriteLock rl = current_connection.getWriteLockObject();
    handleDisconnection(removed_connection.connection_identifier);
    current_connection() = NULL;
    return 0;
}

void ExternalUnitClientEndpoint::closeConnection(const std::string& connection_identifier) {
    LExternalUnitConnectionReadLock rl =  current_connection.getReadLockObject();
    current_connection()->closeConnection();
}

int ExternalUnitClientEndpoint::sendError(const std::string& connection_identifier,
                                    int code,
                                    const std::string& message,
                                    const std::string& domain) {
    return sendMessage(ChaosMoveOperator(encodeError(code,
                                                     message,
                                                     domain)));
}

int ExternalUnitClientEndpoint::sendError(const std::string& connection_identifier,
                                    const chaos::CException& ex) {
    return sendMessage(ChaosMoveOperator(encodeError(ex.errorCode,
                                                     ex.errorMessage,
                                                     ex.errorDomain)));
}

