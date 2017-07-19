/*
 *	CDataWrapperIODriver.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 19/07/2017 INFN, National Institute of Nuclear Physics
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

#include <chaos/cu_toolkit/driver_manager/driver/CDataWrapperIODriver.h>


#define INFO    INFO_LOG(CDataWrapperIODriver)
#define DBG		DBG_LOG(CDataWrapperIODriver)
#define ERR		ERR_LOG(CDataWrapperIODriver)
using namespace chaos::cu::driver_manager::driver;


DEFAULT_CU_DRIVER_PLUGIN_CONSTRUCTOR(CDataWrapperIODriver),
ExternalUnitEndpoint(){}

CDataWrapperIODriver::~CDataWrapperIODriver() {}

void CDataWrapperIODriver::driverInit(const char *initParameter) throw(chaos::CException) {
    INFO << "Init driver:"<<initParameter;
    //! end point identifier
    ExternalUnitEndpoint::endpoint_identifier = "External endpoint of the driver";
}

void CDataWrapperIODriver::driverDeinit() throw(chaos::CException) {
    INFO << "Deinit driver";
}

void CDataWrapperIODriver::handleNewConnection(const std::string& connection_identifier) {
    
}

void CDataWrapperIODriver::handleDisconnection(const std::string& connection_identifier) {
    
}

int CDataWrapperIODriver::handleReceivedeMessage(const std::string& connection_identifier,
                                                 ChaosUniquePtr<chaos::common::data::CDataWrapper> message) {
    return 0;
}

//! Execute a command
MsgManagmentResultType::MsgManagmentResult CDataWrapperIODriver::execOpcode(DrvMsgPtr cmd) {
    MsgManagmentResultType::MsgManagmentResult result = MsgManagmentResultType::MMR_EXECUTED;
    switch(cmd->opcode) {
            //        case BasicIODriverOpcode_READ:{
            //            int ch=cmd->parm[0];
            //            int sizeb=cmd->parm[1];
            //            cmd->resultDataLength= sizeb;
            //            BasicIODriverLDBG_<<"Read add:"<<ch<<" size:"<<sizeb;
            //            cmd->ret=read(cmd->resultData,ch,sizeb);
            //            break;
            //        }
            
        default:
            break;
    }
    return result;
}
