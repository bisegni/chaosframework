/*
 *	ExternalEchoEndpoint.cpp
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

#include <chaos/common/global.h>
#include <chaos/cu_toolkit/external_gateway/ExternalEchoEndpoint.h>

using namespace chaos::cu::external_gateway;

#define INFO    INFO_LOG(ExternalEchoEndpoint)
#define DBG     DBG_LOG(ExternalEchoEndpoint)
#define ERR     ERR_LOG(ExternalEchoEndpoint)

ExternalEchoEndpoint::ExternalEchoEndpoint():
ExternalUnitEndpoint("/echo"),
message_counter(1){}

ExternalEchoEndpoint::~ExternalEchoEndpoint() {}

void ExternalEchoEndpoint::handleNewConnection(const std::string& connection_identifier) {
    INFO << CHAOS_FORMAT("Received new connection %1%", %connection_identifier);
}

void ExternalEchoEndpoint::handleDisconnection(const std::string& connection_identifier) {
    INFO << CHAOS_FORMAT("Connection %1% has been closed", %connection_identifier);
}

int ExternalEchoEndpoint::handleReceivedeMessage(const std::string& connection_identifier,
                                                 chaos::common::data::CDWUniquePtr message) {
    INFO << CHAOS_FORMAT("Received connection from %1% with data '%2%'", %connection_identifier%message->getJSONString());
    if(((message_counter++)%3) ==0) {
        closeConnection(connection_identifier);
    } else {
        message->addStringValue("ExternalEchoEndpoint", "echo answer");
        sendMessage(connection_identifier, ChaosMoveOperator(message));
    }
    return 0;
}
