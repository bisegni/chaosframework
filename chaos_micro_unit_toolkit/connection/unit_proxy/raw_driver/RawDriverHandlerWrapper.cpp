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

#include <chaos_micro_unit_toolkit/connection/unit_proxy/raw_driver/RawDriverHandlerWrapper.h>
#include <iostream>
#include <cassert>
using namespace chaos::micro_unit_toolkit::data;
using namespace chaos::micro_unit_toolkit::connection;
using namespace chaos::micro_unit_toolkit::connection::unit_proxy::raw_driver;

RawDriverHandlerWrapper::RawDriverHandlerWrapper(UnitProxyHandler handler,
                                                 void *user_data,
                                                 const std::string& _authorization_key,
                                                 ChaosUniquePtr<RawDriverUnitProxy>& _u_proxy):
UnitProxyHandlerWrapper(handler,
                        user_data,
                        ChaosMoveOperator(_u_proxy)),
authorization_key(_authorization_key),
auth_state(base_unit->getAuthorizationState()),
authorized(false){}

RawDriverHandlerWrapper::~RawDriverHandlerWrapper(){}

int RawDriverHandlerWrapper::unitEventLoop() {
    int err = 0;
    
    switch (auth_state) {
        case AuthorizationStateUnknown:
            authorized = false;
            base_unit->authorization(authorization_key);
            break;
        case AuthorizationStateRequested:
            base_unit->manageAutorizationPhase();
            break;
        case AuthorizationStateDenied:
            err = callHandler(UP_EV_AUTH_REJECTED, NULL);
            return -1;
        case AuthorizationStateOk:
            if(authorized == false){
                err = callHandler(UP_EV_AUTH_ACCEPTED, NULL);
                authorized = true;
            }
            //manage request by remote user
            err = manageRemoteMessage();
            //perform idle operation
            if(!err){
                err = callHandler(UP_EV_USR_ACTION, NULL);
            }
            break;
    }
    return err;
}

int RawDriverHandlerWrapper::manageRemoteMessage() {
    int err = 0;
    RawDriverUnitProxy * const rd = static_cast<RawDriverUnitProxy*>(base_unit.get());
    RemoteMessageUniquePtr remote_message;
    while(rd->hasMoreMessage()) {
        remote_message = rd->getNextMessage();
        if(remote_message->is_request &&
           remote_message->request_message.get()) {
            if(remote_message->request_message->hasKey("opcode") &&
               remote_message->request_message->isInt32("opcode")){
                UPRequest req = {remote_message->request_message, ChaosUniquePtr<DataPack>(new DataPack())};
                if((err = callHandler(UP_EV_REQ_RECEIVED,
                                      &req)) == 0) {
                    rd->sendAnswer(remote_message, req.response);
                }
            } else {
                //message lakc of opcode key or it is not int32
            }
        } else if(remote_message->isError() == false){
            UPMessage msg = {remote_message->request_message};
            err = callHandler(UP_EV_MSG_RECEIVED, &msg);
        } else {
            UPError err_msg = {remote_message->getErrorCode(),
                remote_message->getErrorMessage(),
                remote_message->getErrorDomain()};
            err = callHandler(UP_EV_ERR_RECEIVED, &err);
        }
    }
    return err;
}
