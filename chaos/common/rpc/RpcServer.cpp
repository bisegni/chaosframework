/*
 *	RpcServer.cpp
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
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
#include <chaos/common/rpc/RpcServer.h>

using namespace chaos;

RpcServer::RpcServer(const std::string& alias):
NamedService(alias),
port_number(0),
command_handler(NULL){}

/*!
 Return the published port
 */
int RpcServer::getPublishedPort() {
    return port_number;
}

/*
 set the command dispatcher associated to the instance of rpc adapter
 */
void RpcServer::setCommandDispatcher(RpcServerHandler *new_command_handler) {
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