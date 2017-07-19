/*
 *	CDataWrapperIODriver.h
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

#ifndef __CHAOSFramework__CE5788A_1D68_477E_BB9E_55BE8F7D3373_CDataWrapperIODriver_h
#define __CHAOSFramework__CE5788A_1D68_477E_BB9E_55BE8F7D3373_CDataWrapperIODriver_h

#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>

#include <chaos/cu_toolkit/external_gateway/ExternalUnitEndpoint.h>

namespace chaos {
    namespace cu {
        namespace driver_manager {
            namespace driver {
                
                class CDataWrapperIODriver:
                ADD_CU_DRIVER_PLUGIN_SUPERCLASS,
                chaos::cu::external_gateway::ExternalUnitEndpoint {
                    void driverInit(const char *initParameter) throw (chaos::CException);
                    void driverDeinit() throw (chaos::CException);
                protected:
                protected:
                    //!inherited method by @ExternalUnitEndpoint
                    void handleNewConnection(const std::string& connection_identifier);
                    //!inherited method by @ExternalUnitEndpoint
                    void handleDisconnection(const std::string& connection_identifier);
                    
                    int handleReceivedeMessage(const std::string& connection_identifier,
                                               ChaosUniquePtr<chaos::common::data::CDataWrapper> message);
                public:
                    CDataWrapperIODriver();
                    
                    ~CDataWrapperIODriver();
                    //! Execute a command
                    chaos::cu::driver_manager::driver::MsgManagmentResultType::MsgManagmentResult execOpcode(chaos::cu::driver_manager::driver::DrvMsgPtr cmd);
                };
                
            }
        }
    }
}

#endif /* __CHAOSFramework__CE5788A_1D68_477E_BB9E_55BE8F7D3373_CDataWrapperIODriver_h */
