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
#include <chaos/common/rpc/RpcServer.h>

using namespace chaos;

RpcServer::RpcServer(const std::string& alias):
NamedService(alias),
port_number(0),
command_handler(NULL){}

void RpcServer::setAlternatePortAddress(int new_port_address) {
    port_number = new_port_address;
}

/*!
 Return the published port
 */
int RpcServer::getPublishedPort() {
    return port_number;
}

/*
 set the command dispatcher associated to the instance of rpc adapter
 */
void RpcServer::setCommandDispatcher(chaos::common::rpc::RpcServerHandler *new_command_handler) {
    command_handler = new_command_handler;
}

//! return the numebr of message that are waiting to be sent
/*!
 driver can overload this method to return(if has any) the size
 of internal queue message
 */
uint64_t RpcServer::getMessageQueueSize() {
    return 0;
}
