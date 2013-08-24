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
#ifndef __ControlUnitTest__TestDeviceDriver__
#define __ControlUnitTest__TestDeviceDriver__

#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>

namespace cu_driver = chaos::cu::driver_manager::driver;

DEFINE_CU_DRIVER_DEFINITION_PROTOTYPE(DummyDriver)

/*
 driver definition
 */
class DummyDriver: ADD_CU_DRIVER_PLUGIN_SUPERCLASS {

	
	void driverInit(const char *initParameter) throw(chaos::CException);
	void driverDeinit() throw(chaos::CException);
public:
    DummyDriver();
	~DummyDriver();
    //! Execute a command
	cu_driver::MsgManagmentResultType::MsgManagmentResult execOpcode(cu_driver::DrvMsgPtr cmd);
};

#endif /* defined(__ControlUnitTest__DummyDriver__) */
