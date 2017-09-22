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

#include "AbstractExternalCommand.h"

using namespace chaos::agent::external_command_pipe;

using namespace chaos::common::message;
using namespace chaos::common::utility;

AbstractExternalCommand::AbstractExternalCommand(const std::string& service_name,
                                                 AbstractCommandOutputStream& _cmd_ostream,
                                                 MDSMessageChannel& _mds_message_channel):
NamedService(service_name),
cmd_ostream(_cmd_ostream),
mds_message_channel(_mds_message_channel){}

AbstractExternalCommand::~AbstractExternalCommand() {}

void AbstractExternalCommand::print(const std::string& line) {
    cmd_ostream << line;
}

void AbstractExternalCommand::println(const std::string& line) {
    cmd_ostream << line << "\n";
}
