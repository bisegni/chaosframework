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

#include <chaos/cu_toolkit/driver_manager/driver/OpcodeExternalCommandMapper.h>
using namespace chaos::cu::driver_manager::driver;

OpcodeExternalCommandMapper::OpcodeExternalCommandMapper(RemoteIODriverProtocol *_driver_protocol):
driver_protocol(_driver_protocol){CHAOS_ASSERT(driver_protocol)}

OpcodeExternalCommandMapper::~OpcodeExternalCommandMapper() {}

void OpcodeExternalCommandMapper::driverInit(const chaos::common::data::CDataWrapper& init_parameter) throw(chaos::CException) {}

void OpcodeExternalCommandMapper::driverDeinit() throw(chaos::CException) {}

int OpcodeExternalCommandMapper::sendRawRequest(chaos::common::data::CDWUniquePtr message_data,
                                                chaos::common::data::CDWShrdPtr& message_response,
                                                uint32_t timeout) {
    CHAOS_ASSERT(driver_protocol)
    return driver_protocol->sendRawRequest(ChaosMoveOperator(message_data),
                                           message_response,
                                           timeout);
}

int  OpcodeExternalCommandMapper::sendOpcodeRequest(const std::string& opcode,
                                                    chaos::common::data::CDWUniquePtr opcode_parameter,
                                                    chaos::common::data::CDWShrdPtr& message_response,
                                                    uint32_t timeout ) {
    CHAOS_ASSERT(driver_protocol)
    return driver_protocol->sendOpcodeRequest(opcode,
                                              ChaosMoveOperator(opcode_parameter),
                                              message_response,timeout);
}

int OpcodeExternalCommandMapper::sendRawMessage(chaos::common::data::CDWUniquePtr message_data) {
    CHAOS_ASSERT(driver_protocol)
    return driver_protocol->sendRawMessage(ChaosMoveOperator(message_data));
}

int  OpcodeExternalCommandMapper::sendOpcodeMessage(const std::string& opcode,
                                                    chaos::common::data::CDWUniquePtr opcode_message) {
    CHAOS_ASSERT(driver_protocol)
    return driver_protocol->sendOpcodeMessage(opcode,
                                              ChaosMoveOperator(opcode_message));
}

int OpcodeExternalCommandMapper::sendInitRequest() {
    CHAOS_ASSERT(driver_protocol)
    return driver_protocol->sendInitRequest();
}

int OpcodeExternalCommandMapper::sendDeinitRequest() {
    CHAOS_ASSERT(driver_protocol)
    return driver_protocol->sendDeinitRequest();
}
