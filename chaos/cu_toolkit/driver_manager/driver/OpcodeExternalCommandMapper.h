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

#ifndef __CHAOSFramework__B51A442_6610_45C9_9855_E6E15E2F3DA2_OpcodeExternalCommandMapper_h
#define __CHAOSFramework__B51A442_6610_45C9_9855_E6E15E2F3DA2_OpcodeExternalCommandMapper_h

#include <chaos/common/data/CDataWrapper.h>
#include <chaos/cu_toolkit/driver_manager/driver/DriverTypes.h>
#include <chaos/cu_toolkit/driver_manager/driver/RemoteIODriverProtocol.h>

namespace chaos {
    namespace cu {
        namespace driver_manager {
            namespace driver {
                //!Opcode Ext driver message mapper
                /*!
                 this is the base class that permit to map the opcode sent by Controlunit
                 to the external driver command
                 */
                class OpcodeExternalCommandMapper:
                public OpcodeExecutor {
                    RemoteIODriverProtocol *driver_protocol;
                protected:
                    //!//!Wrap to @RemoteIODriverProtocol::sendRawRequest method
                    int sendRawRequest(chaos::common::data::CDWUniquePtr message_data,
                                       chaos::common::data::CDWShrdPtr& message_response,
                                       uint32_t timeout = REMOTE_DRIVER_DEFAULT_TIMEOUT);
                    //!Wrap to @RemoteIODriverProtocol::sendOpcodeRequest method
                    int sendOpcodeRequest(const std::string& opcode,
                                          chaos::common::data::CDWUniquePtr opcode_parameter,
                                          chaos::common::data::CDWShrdPtr& message_response,
                                          uint32_t timeout = REMOTE_DRIVER_DEFAULT_TIMEOUT);
                    //!Wrap to @RemoteIODriverProtocol::sendRawMessage method
                    int sendRawMessage(chaos::common::data::CDWUniquePtr message_data);
                    //!Wrap to @RemoteIODriverProtocol::sendOpcodeMessage method
                    int sendOpcodeMessage(const std::string& opcode,
                                          chaos::common::data::CDWUniquePtr opcode_message);
                    //!Wrap to @RemoteIODriverProtocol::sendInitRequest method
                    int sendInitRequest();
                    //!Wrap to @RemoteIODriverProtocol::sendDeinitRequest method
                    int sendDeinitRequest();
                public:
                    OpcodeExternalCommandMapper(RemoteIODriverProtocol *driver_protocol);
                    
                    virtual ~OpcodeExternalCommandMapper();
                    
                    virtual void driverInit(const chaos::common::data::CDataWrapper& init_parameter) throw(chaos::CException);
                    
                    virtual void driverDeinit() throw(chaos::CException);
                    
                    virtual int asyncMessageReceived(chaos::common::data::CDWUniquePtr message) = 0;
                };
                
            }
        }
    }
}

#endif /* __CHAOSFramework__B51A442_6610_45C9_9855_E6E15E2F3DA2_OpcodeExternalCommandMapper_h */
