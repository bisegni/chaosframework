/*
 *	RemoteIODriver.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 25/07/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__8328273_33D4_43F5_8FD4_B0B6493427DC_RemoteIODriver_h
#define __CHAOSFramework__8328273_33D4_43F5_8FD4_B0B6493427DC_RemoteIODriver_h

#include <chaos/cu_toolkit/driver_manager/driver/AbstractRemoteIODriver.h>
#include <chaos/cu_toolkit/driver_manager/driver/OpcodeExternalCommandMapper.h>
namespace chaos {
    namespace cu {
        namespace driver_manager {
            namespace driver {
                //!permit to specify the converter to adapt already created driver for be used as external
                template<typename OpExCMDAdaptor>
                class TemplatedRemoteIODriver:
                public AbstractRemoteIODriver {
                    ChaosUniquePtr<OpcodeExternalCommandMapper> opcode_ext_cmd_mapper;
                    virtual MsgManagmentResultType::MsgManagmentResult execOpcode(DrvMsgPtr cmd) {
                        return opcode_ext_cmd_mapper->execOpcode(cmd);
                    }
                    
                    int asyncMessageReceived(chaos::common::data::CDWUniquePtr message) {
                        return opcode_ext_cmd_mapper->asyncMessageReceived(ChaosMoveOperator(message));
                    }
                public:
                    TemplatedRemoteIODriver():
                    opcode_ext_cmd_mapper(new OpExCMDAdaptor(this)){}
                    ~TemplatedRemoteIODriver() {}
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__8328273_33D4_43F5_8FD4_B0B6493427DC_RemoteIODriver_h */
