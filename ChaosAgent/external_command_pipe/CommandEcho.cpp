/*
 *	CommandEcho.cpp
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
