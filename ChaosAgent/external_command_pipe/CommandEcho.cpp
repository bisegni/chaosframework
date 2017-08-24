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

#include "CommandEcho.h"

using namespace chaos::agent::external_command_pipe;

CommandEcho::CommandEcho(AbstractCommandOutputStream& _cmd_ostream,
                         common::message::MDSMessageChannel& _mds_message_channel):
AbstractExternalCommand("ECHO",
                        _cmd_ostream,
                        _mds_message_channel){}

CommandEcho::~CommandEcho(){}

int CommandEcho::execute(ChaosStringVector input_parameter) {
    if(input_parameter.size() != 2) {
        println("Invalid parameter number");
        return 1;
    }
    println(input_parameter[1]);
    return 0;
}
