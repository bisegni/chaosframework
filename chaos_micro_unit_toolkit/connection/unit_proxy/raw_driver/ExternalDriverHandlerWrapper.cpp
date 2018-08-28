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

#include <chaos_micro_unit_toolkit/connection/unit_proxy/raw_driver/ExternalDriverHandlerWrapper.h>
#include <iostream>
#include <cassert>
using namespace chaos::micro_unit_toolkit::data;
using namespace chaos::micro_unit_toolkit::connection;
using namespace chaos::micro_unit_toolkit::connection::unit_proxy::raw_driver;


#define MSG_KEY                 "msg"
#define OPCODE_KEY              "opc"
#define OPCODE_PARAMETER_KEY    "par"
#define INITIALIZATION_URI      "uri"
#define INIT_OPCODE             "init"
#define DEINIT_OPCODE           "deinit"
#define CONFIGURATION_STATE     "configuration_state"

ExternalDriverHandlerWrapper::ExternalDriverHandlerWrapper(UnitProxyHandler handler,
                                                           void *user_data,
                                                           ChaosUniquePtr<ExternalDriverUnitProxy>& _u_proxy):
UnitProxyHandlerWrapper(handler,
                        user_data,
#if __cplusplus >= 201103L
                        MOVE(_u_proxy)
#else
                        MOVE(ChaosUniquePtr<AbstractUnitProxy>(_u_proxy))
#endif
                        ),
unit_state(base_unit->getUnitState()),
authorized(false){}

ExternalDriverHandlerWrapper::~ExternalDriverHandlerWrapper(){}

int ExternalDriverHandlerWrapper::sendMessage(data::CDWUniquePtr& message_data) {
    ExternalDriverUnitProxy * const rd = static_cast<ExternalDriverUnitProxy*>(base_unit.get());
    return rd->sendMessage(message_data);
}

int ExternalDriverHandlerWrapper::unitEventLoop() {
    int err = 0;
    switch (unit_state) {
        case UnitStateUnknown:
            authorized = false;
            static_cast<ExternalDriverUnitProxy*>(base_unit.get())->manageAuthenticationRequest();
            break;
        case UnitStateNotAuthenticated:
            err = callHandler(UP_EV_AUTH_REJECTED, NULL);
            break;
        case UnitStateAuthenticated:
            if(authorized == false){
                err = callHandler(UP_EV_AUTH_ACCEPTED, NULL);
                authorized = true;
            }
            //manage request by remote user
            err = manageRemoteMessage();
            //perform idle operation
            if(!err) {
                //call handler for user action passing the instace as public interface
                err = callHandler(UP_EV_USR_ACTION, this);
            }
            break;
    }
    return err;
}

int ExternalDriverHandlerWrapper::manageRemoteMessage() {
    int err = 0;
    bool init_opcode = false;
    bool deinit_opcode = false;
    ExternalDriverUnitProxy * const rd = static_cast<ExternalDriverUnitProxy*>(base_unit.get());
    RemoteMessageUniquePtr remote_message;
    while(rd->hasMoreMessage()) {
        remote_message = rd->getNextMessage();
        if(remote_message->message.get() == NULL) return -1;
        std::cout << remote_message->message->toString();
        std::string uri;
        std::string opcode;
        data::CDWShrdPtr r_msg;
        if(remote_message->message->hasKey(OPCODE_KEY) &&
           remote_message->message->isStringValue(OPCODE_KEY)){
            opcode = remote_message->message->getStringValue(OPCODE_KEY);
        } else {
            return 0;
        }
        
        if(remote_message->message->hasKey(INITIALIZATION_URI) &&
           remote_message->message->isStringValue(INITIALIZATION_URI)){
            uri = remote_message->message->getStringValue(INITIALIZATION_URI);
        } else {
            return 0;
        }
        
        if(remote_message->message->hasKey(OPCODE_PARAMETER_KEY) &&
           remote_message->message->isCDWValue(OPCODE_PARAMETER_KEY)){
            r_msg = remote_message->message->getCDWValue(OPCODE_PARAMETER_KEY);
        }
        //compose message and requests
        EDMessage req_message = {uri, opcode, r_msg};
        //check if we have and init msg
        if((init_opcode = opcode.compare(INIT_OPCODE) == 0)) {
            EDInitRequest init_request = {req_message,{}};
            if((err = callHandler(UP_EV_INIT_RECEIVED,
                                  &init_request)) == 0) {
                CDWUniquePtr response_msg = composeResponseMessage(init_request.response);
                response_msg->addInt32Value(CONFIGURATION_STATE, init_request.response.configuration_state);
                response_msg->addStringValue(INITIALIZATION_URI, init_request.response.new_uri_id);
                rd->sendAnswer(remote_message, response_msg);
            }
        } else if((deinit_opcode = opcode.compare(DEINIT_OPCODE) == 0)) {
            EDDeinitRequest deinit_request = {req_message,{}};
            if((err = callHandler(UP_EV_DEINIT_RECEIVED,
                                  &deinit_request)) == 0) {
                rd->sendAnswer(remote_message, composeResponseMessage(deinit_request.response));
            }
        } else if(remote_message->is_request) {
            EDNormalRequest normal_request = {req_message,{}};
            if((err = callHandler(UP_EV_REQ_RECEIVED,
                                  &normal_request)) == 0) {
                CDWUniquePtr response_msg = composeResponseMessage(normal_request.response);
                normal_request.response.message->copyAllTo(*response_msg);
                rd->sendAnswer(remote_message, response_msg);
                
            }
        } else {
            err = callHandler(UP_EV_MSG_RECEIVED, &req_message);
        }
        
    }
    return err;
}

CDWUniquePtr ExternalDriverHandlerWrapper::composeResponseMessage(EDResponse& base_resposne) {
    CDWUniquePtr response_msg(new DataPack());
    response_msg->addInt32Value(ERR_CODE, base_resposne.error_code);
    if(base_resposne.error_message.size()) {
        response_msg->addStringValue(ERR_MSG, base_resposne.error_message);
    }
    if(base_resposne.error_domain.size()) {
        response_msg->addStringValue(ERR_MSG, base_resposne.error_message);
    }
    return response_msg;
}
