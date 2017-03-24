/*
 *	AbstractExternalCommand.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 24/03/2017 INFN, National Institute of Nuclear Physics
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
