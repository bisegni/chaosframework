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

#ifndef __CHAOSFramework__8328273_33D4_43F5_8FD4_B0B6493427DC_RemoteIODriver_h
#define __CHAOSFramework__8328273_33D4_43F5_8FD4_B0B6493427DC_RemoteIODriver_h

#include <chaos/cu_toolkit/driver_manager/driver/AbstractRemoteIODriver.h>
#include <chaos/cu_toolkit/driver_manager/driver/AbstractServerRemoteIODriver.h>
#include <chaos/cu_toolkit/driver_manager/driver/AbstractClientRemoteIODriver.h>
#include <chaos/cu_toolkit/driver_manager/driver/AbstractRemoteIODriver.h>
#include <chaos/cu_toolkit/driver_manager/driver/OpcodeExternalCommandMapper.h>
#include <chaos/cu_toolkit/driver_manager/driver/RemoteIODriverProtocol.h>
namespace chaos {
    namespace cu {
        namespace driver_manager {
            namespace driver {
                //!permit to specify the converter to adapt already created driver for be used as external
                template<typename OpExCMDAdaptor, typename ExtDriverImpl>
                class OpcodeDriverWrapper:
                public ExtDriverImpl,
                public RemoteIODriverProtocol {
                    ChaosUniquePtr<OpcodeExternalCommandMapper> opcode_ext_cmd_mapper;
                    virtual MsgManagmentResultType::MsgManagmentResult execOpcode(DrvMsgPtr cmd) {
                        return opcode_ext_cmd_mapper->execOpcode(cmd);
                    }
                    
                    int asyncMessageReceived(chaos::common::data::CDWUniquePtr message) {
                        return opcode_ext_cmd_mapper->asyncMessageReceived(ChaosMoveOperator(message));
                    }
                    
                    
                public:
                    OpcodeDriverWrapper():
                    opcode_ext_cmd_mapper(new OpExCMDAdaptor(this)){}
                    ~OpcodeDriverWrapper() {}
                    
                    void driverInit(const chaos::common::data::CDataWrapper& init_parameter) throw(chaos::CException) {
                        ExtDriverImpl::driverInit(init_parameter);
                        /* cannot be called before the instance is created*/
                        opcode_ext_cmd_mapper->driverInit(init_parameter);
                    }
                    
                    void driverDeinit() throw(chaos::CException) {
                        opcode_ext_cmd_mapper->driverDeinit();
                        ExtDriverImpl::driverDeinit();
                    }
                    
                    //!Inherited by RemoteIODriverProtocol::sendRawRequest
                    int sendRawRequest(chaos::common::data::CDWUniquePtr message_data,
                                       chaos::common::data::CDWShrdPtr& message_response,
                                       uint32_t timeout = 5000) {
                        return ExtDriverImpl::sendRawRequest(ChaosMoveOperator(message_data),
                                                             message_response,
                                                             timeout);
                    }
                    
                    //!Inherited by RemoteIODriverProtocol::sendOpcodeRequest
                    int sendOpcodeRequest(const std::string opcode,
                                          chaos::common::data::CDWUniquePtr opcode_parameter,
                                          chaos::common::data::CDWShrdPtr& message_response) {
                        return ExtDriverImpl::sendOpcodeRequest(opcode,
                                                                ChaosMoveOperator(opcode_parameter),
                                                                message_response,
                                                                timeout);
                    }
                    
                    //!Inherited by RemoteIODriverProtocol::sendRawMessage
                    int sendRawMessage(chaos::common::data::CDWUniquePtr message_data) {
                        return ExtDriverImpl::sendRawMessage(ChaosMoveOperator(message_data));
                    }
                    
                    //!Inherited by RemoteIODriverProtocol::sendOpcodeMessage
                    int sendOpcodeMessage(const std::string opcode,
                                          chaos::common::data::CDWUniquePtr opcode_message) {
                        return ExtDriverImpl::sendOpcodeMessage(opcode,
                                                                ChaosMoveOperator(opcode_message));
                    }
                };
                
                //marcro for helping in driver declaration
#define EXTERNAL_CLIENT_SERVER_DRIVER_CLASS_DEFINITION(DriverNamePrefix, OpcodeLogicWrapper)\
DEFINE_CU_DRIVER_DEFINITION_PROTOTYPE(DriverNamePrefix ## RemoteServerDriver)\
class DriverNamePrefix ## RemoteServerDriver:\
public chaos::cu::driver_manager::driver::OpcodeDriverWrapper<OpcodeLogicWrapper, chaos::cu::driver_manager::driver::AbstractServerRemoteIODriver> {};\
DEFINE_CU_DRIVER_DEFINITION_PROTOTYPE(DriverNamePrefix ## RemoteClientDriver)\
class DriverNamePrefix ## RemoteClientDriver:\
public chaos::cu::driver_manager::driver::OpcodeDriverWrapper<OpcodeLogicWrapper, chaos::cu::driver_manager::driver::AbstractClientRemoteIODriver> {};
                
            }
        }
    }
}

#endif /* __CHAOSFramework__8328273_33D4_43F5_8FD4_B0B6493427DC_RemoteIODriver_h */
