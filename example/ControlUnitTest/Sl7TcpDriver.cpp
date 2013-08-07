/*
 *	Sl7TcpDriver.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2013 INFN, National Institute of Nuclear Physics
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
#include "Sl7TcpDriver.h"

#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>

namespace cu_driver = chaos::cu::driver_manager::driver;


//GET_PLUGIN_CLASS_DEFINITION
//we need only to define the driver because we don't are makeing a plugin
OPEN_CU_DRIVER_PLUGIN_CLASS_DEFINITION(Sl7TcpDriver, 1.0.0, Sl7TcpDriver)
REGISTER_CU_DRIVER_PLUGIN_CLASS_INIT_ATTRIBUTE(Sl7Drv,http_address:port)
CLOSE_CU_DRIVER_PLUGIN_CLASS_DEFINITION

//default constructor definition
DEFAULT_CU_DRIVER_PLUGIN_CONSTRUCTOR(Sl7TcpDriver) {
	
}

Sl7TcpDriver::~Sl7TcpDriver() {
	
}

//! Execute a command
void Sl7TcpDriver::execOpcode(cu_driver::DrvMsgPtr cmd) {
	switch (cmd->opcode) {
		case cu_driver::OpcodeType::OP_START:
			
			break;

	}
}