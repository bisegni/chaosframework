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

using namespace chaos::common::data;
using namespace chaos::cu::driver_manager::driver;

#define DBG DBG_LOG(OpcodeExternalCommandMapper)

#define WRITE_ERR_ON_CMD(r, c, m, d)\
cmd->ret = c;\
snprintf(cmd->err_msg, 255, "%s", m);\
snprintf(cmd->err_dom, 255, "%s", d);

#define RETURN_ERROR(r, c, m, d)\
WRITE_ERR_ON_CMD(r, c, m, d)\
return cmd->ret;

#define SEND_REQUEST_OPC(opc,c, r,a) {\
int err;\
try { \
if((err=sendOpcodeRequest(opc,ChaosMoveOperator(r),a))) {\
WRITE_ERR_ON_CMD(err, -1, "Error from from remote driver", __PRETTY_FUNCTION__);\
}else {\
if(response->hasKey("err")) {\
if(response->isInt32Value("err") == false)  {\
WRITE_ERR_ON_CMD(c, -3, "'err' key need to be an int32 value", __PRETTY_FUNCTION__);\
} else {\
c->ret = response->getInt32Value("err");\
}\
} else {\
WRITE_ERR_ON_CMD(c, -2, "'err' key not found on external driver return package", __PRETTY_FUNCTION__);\
}}\
}\
catch (...) \
{	\
DBG << "Exception catched in send request opc" ; \
WRITE_ERR_ON_CMD(err, -6, "Exception from remote driver", __PRETTY_FUNCTION__);\
}\
}

OpcodeExternalCommandMapper::OpcodeExternalCommandMapper(RemoteIODriverProtocol *_driver_protocol):
driver_protocol(_driver_protocol){CHAOS_ASSERT(driver_protocol)}
OpcodeExternalCommandMapper::~OpcodeExternalCommandMapper() {}

void OpcodeExternalCommandMapper::driverInit(const chaos::common::data::CDataWrapper& init_parameter)  {}
void OpcodeExternalCommandMapper::driverDeinit()  {}

int OpcodeExternalCommandMapper::sendInit(chaos::cu::driver_manager::driver::DrvMsgPtr cmd){
    common::data::CDWShrdPtr response;
    common::data::CDWUniquePtr init_pack(new CDataWrapper());
    cmd->ret =  0;
    SEND_REQUEST_OPC("init",cmd, init_pack, response);
    return cmd->ret;
}

int OpcodeExternalCommandMapper::sendDeinit(chaos::cu::driver_manager::driver::DrvMsgPtr cmd){
    common::data::CDWShrdPtr response;
    common::data::CDWUniquePtr init_pack(new CDataWrapper());
    cmd->ret =  0;
    SEND_REQUEST_OPC("deinit",cmd, init_pack, response);
    return cmd->ret;
}

#pragma mark Protected Method

int OpcodeExternalCommandMapper::sendRawRequest(chaos::common::data::CDWUniquePtr message_data,
                                                chaos::common::data::CDWShrdPtr& message_response,
                                                uint32_t timeout) {
    CHAOS_ASSERT(driver_protocol)
    return driver_protocol->sendRawRequest(MOVE(message_data),
                                           message_response,
                                           timeout);
}

int  OpcodeExternalCommandMapper::sendOpcodeRequest(const std::string& opcode,
                                                    chaos::common::data::CDWUniquePtr opcode_parameter,
                                                    chaos::common::data::CDWShrdPtr& message_response,
                                                    uint32_t timeout ) {
    CHAOS_ASSERT(driver_protocol)
    return driver_protocol->sendOpcodeRequest(opcode,
                                              MOVE(opcode_parameter),
                                              message_response,timeout);
}

int OpcodeExternalCommandMapper::sendRawMessage(chaos::common::data::CDWUniquePtr message_data) {
    CHAOS_ASSERT(driver_protocol)
    return driver_protocol->sendRawMessage(MOVE(message_data));
}

int  OpcodeExternalCommandMapper::sendOpcodeMessage(const std::string& opcode,
                                                    chaos::common::data::CDWUniquePtr opcode_message) {
    CHAOS_ASSERT(driver_protocol)
    return driver_protocol->sendOpcodeMessage(opcode,
                                              MOVE(opcode_message));
}

int OpcodeExternalCommandMapper::sendInitRequest() {
    CHAOS_ASSERT(driver_protocol)
    return driver_protocol->sendInitRequest();
}

int OpcodeExternalCommandMapper::sendDeinitRequest() {
    CHAOS_ASSERT(driver_protocol)
    return driver_protocol->sendDeinitRequest();
}

void OpcodeExternalCommandMapper::fillMessageWithDeviceParam(DrvMsgPtr cmd,
                                                             CDataWrapper& message) const {
    if(!cmd ||
       !cmd->device_param) {return;}
    
    //we have message
    cmd->device_param->copyAllTo(message);
}
