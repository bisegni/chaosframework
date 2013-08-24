/*
 *	DummyDriver.h
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
#include "DummyDriver.h"

#include <string>

#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>

#include <boost/regex.hpp>

namespace cu_driver = chaos::cu::driver_manager::driver;

#define SL7DRVLAPP_		LAPP_ << "[DummyDriver] "
#define SL7DRVLDBG_		LDBG_ << "[DummyDriver] "
#define SL7DRVLERR_		LERR_ << "[DummyDriver] "


//! Regular expression for check server hostname and port
static const boost::regex PlcHostNameAndPort("([a-zA-Z0-9]+(.[a-zA-Z0-9]+)+):([0-9]{3,5})");
//! Regular expression for check server ip and port
static const boost::regex PlcIpAnPort("(\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b):([0-9]{4,5})");


//GET_PLUGIN_CLASS_DEFINITION
//we need only to define the driver because we don't are makeing a plugin
OPEN_CU_DRIVER_PLUGIN_CLASS_DEFINITION(DummyDriver, 1.0.0, DummyDriver)
REGISTER_CU_DRIVER_PLUGIN_CLASS_INIT_ATTRIBUTE(Sl7Drv,http_address/dnsname:port)
CLOSE_CU_DRIVER_PLUGIN_CLASS_DEFINITION

//default constructor definition
DEFAULT_CU_DRIVER_PLUGIN_CONSTRUCTOR(DummyDriver) {
	
}

//default descrutcor
DummyDriver::~DummyDriver() {
	
}

void DummyDriver::driverInit(const char *initParameter) throw(chaos::CException) {
	SL7DRVLAPP_ << "Init dummy driver";
}

void DummyDriver::driverDeinit() throw(chaos::CException) {
	SL7DRVLAPP_ << "Deinit dummy driver";

}

//! Execute a command
cu_driver::MsgManagmentResultType::MsgManagmentResult DummyDriver::execOpcode(cu_driver::DrvMsgPtr cmd) {
	cu_driver::MsgManagmentResultType::MsgManagmentResult result = cu_driver::MsgManagmentResultType::MMR_EXECUTED;
	return result;
}