/*
 * Copyright 2012, 11/10/2017 INFN
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
#include <chaos/common/external_unit/ExternalUnitServerEndpoint.h>

using namespace chaos::common::data;
using namespace chaos::common::external_unit;

ExternalUnitServerEndpoint::ExternalUnitServerEndpoint() {}

ExternalUnitServerEndpoint::ExternalUnitServerEndpoint(const std::string& _endpoint_identifier):
endpoint_identifier(_endpoint_identifier),
number_of_connection_accepted(-1){}

ExternalUnitServerEndpoint::~ExternalUnitServerEndpoint() {}

int ExternalUnitServerEndpoint::sendMessage(const std::string& connection_identifier,
                                      CDWUniquePtr message,
                                      const EUCMessageOpcode opcode) {
    LMapConnectionReadLock rl;
    do {
        rl = map_connection.getReadLockObject(utility::ChaosLockTypeTry);
        usleep(10000);
    }while(rl->owns_lock() == false);
    if(map_connection().count(connection_identifier) == 0) return -1;
    //send data to the coneection
    return map_connection()[connection_identifier]->sendData(ChaosMoveOperator(message),
                                                             opcode);
}

std::string ExternalUnitServerEndpoint::getIdentifier() {
    return endpoint_identifier;
}

int ExternalUnitServerEndpoint::addConnection(ExternalUnitConnection& new_connection) {
    LMapConnectionWriteLock wl;
    do {
        wl = map_connection.getWriteLockObject(utility::ChaosLockTypeTry);
        usleep(10000);
    }while(wl->owns_lock() == false);
    map_connection().insert(MapConnectionPair(new_connection.connection_identifier, &new_connection));
    wl->unlock();
    handleNewConnection(new_connection.connection_identifier);
    return 0;
}

int ExternalUnitServerEndpoint::removeConnection(ExternalUnitConnection& removed_connection) {
    LMapConnectionWriteLock wl = map_connection.getWriteLockObject();
    handleDisconnection(removed_connection.connection_identifier);
    map_connection().erase(removed_connection.connection_identifier);
    return 0;
}

const bool ExternalUnitServerEndpoint::canAcceptMoreConnection() {
    LMapConnectionReadLock rl =  map_connection.getReadLockObject();
    if(number_of_connection_accepted<0) return true;
    else return number_of_connection_accepted > map_connection().size();
}

void ExternalUnitServerEndpoint::closeConnection(const std::string& connection_identifier) {
    LMapConnectionReadLock rl =  map_connection.getReadLockObject();
    map_connection()[connection_identifier]->closeConnection();
}

int ExternalUnitServerEndpoint::sendError(const std::string& connection_identifier,
                                    int code,
                                    const std::string& message,
                                    const std::string& domain) {
    return sendMessage(connection_identifier,
                       ChaosMoveOperator(encodeError(code,
                                                     message,
                                                     domain)));
}

int ExternalUnitServerEndpoint::sendError(const std::string& connection_identifier,
                                    const chaos::CException& ex) {
    return sendMessage(connection_identifier,
                       ChaosMoveOperator(encodeError(ex.errorCode,
                                                     ex.errorMessage,
                                                     ex.errorDomain)));
}

const int ExternalUnitServerEndpoint::getNumberOfAcceptedConnection() const {
    return number_of_connection_accepted;
}

void ExternalUnitServerEndpoint::setNumberOfAcceptedConnection(int _number_of_connection_accepted) {
    number_of_connection_accepted = _number_of_connection_accepted;
}
