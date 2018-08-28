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

#include <chaos/common/global.h>
#include <chaos/common/external_unit/ExternalEchoEndpoint.h>

using namespace chaos::common::external_unit;

#define INFO    INFO_LOG(ExternalEchoEndpoint)
#define DBG     DBG_LOG(ExternalEchoEndpoint)
#define ERR     ERR_LOG(ExternalEchoEndpoint)

ExternalEchoEndpoint::ExternalEchoEndpoint():
ExternalUnitServerEndpoint("/echo"),
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
    message->addStringValue("ExternalEchoEndpoint", "echo answer");
    sendMessage(connection_identifier, ChaosMoveOperator(message));
    return 0;
}
