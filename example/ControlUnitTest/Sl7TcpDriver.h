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
#ifndef __ControlUnitTest__TestDeviceDriver__
#define __ControlUnitTest__TestDeviceDriver__

#include "sl7lib/nodave.h"
#include "sl7lib/openSocket.h"

#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>

namespace cu_driver = chaos::cu::driver_manager::driver;

DEFINE_CU_DRIVER_DEFINITION_PROTOTYPE(Sl7TcpDriver)

/*
 driver definition
 */
class Sl7TcpDriver: ADD_CU_DRIVER_PLUGIN_SUPERCLASS {
    
	//conenction information
	daveInterface *di;
    daveConnection *dc;
    daveFileDescriptors fds;
	
	inline cu_driver::MsgManagmentResultType::MsgManagmentResult initPLCConnection(cu_driver::DrvMsgPtr initPack);
	inline cu_driver::MsgManagmentResultType::MsgManagmentResult deinitPLCConnection();
	
public:
    Sl7TcpDriver();
	~Sl7TcpDriver();
    //! Execute a command
	cu_driver::MsgManagmentResultType::MsgManagmentResult execOpcode(cu_driver::DrvMsgPtr cmd);
};

#endif /* defined(__ControlUnitTest__Sl7TcpDriver__) */
