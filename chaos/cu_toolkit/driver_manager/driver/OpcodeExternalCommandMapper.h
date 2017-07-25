/*
 *	OpcodeExternalCommandMapper.h
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

#ifndef __CHAOSFramework__B51A442_6610_45C9_9855_E6E15E2F3DA2_OpcodeExternalCommandMapper_h
#define __CHAOSFramework__B51A442_6610_45C9_9855_E6E15E2F3DA2_OpcodeExternalCommandMapper_h

#include <chaos/cu_toolkit/driver_manager/driver/AbstractRemoteIODriver.h>

namespace chaos {
    namespace cu {
        namespace driver_manager {
            namespace driver {
                
                //!Opcode Ext driver message mapper
                /*!
                 this is the base class that permit to map the opcode sent by Controlunit
                 to the external driver command
                 */
                class OpcodeExternalCommandMapper {
                    AbstractRemoteIODriver *remote_driver;
                public:
                    OpcodeExternalCommandMapper(AbstractRemoteIODriver *_remote_driver);
                    virtual ~OpcodeExternalCommandMapper();
                    virtual MsgManagmentResultType::MsgManagmentResult execOpcode(DrvMsgPtr cmd) = 0;
                };
                
            }
        }
    }
}

#endif /* __CHAOSFramework__B51A442_6610_45C9_9855_E6E15E2F3DA2_OpcodeExternalCommandMapper_h */
