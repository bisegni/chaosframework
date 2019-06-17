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

#include <chaos_micro_unit_toolkit/connection/unit_proxy/UnitProxyHandlerWrapper.h>
#include <iostream>
#ifndef _WIN32
#include <unistd.h>
#endif
#include <chaos/cu_toolkit/windowsCompliant.h>
#include <cassert>

using namespace chaos::micro_unit_toolkit::data;
using namespace chaos::micro_unit_toolkit::connection::unit_proxy;
using namespace chaos::micro_unit_toolkit::connection::connection_adapter;

UnitProxyHandlerWrapper::UnitProxyHandlerWrapper(UnitProxyHandler _handler,
                                                 void *_user_data,
                                                 ChaosUniquePtr<AbstractUnitProxy> _base_unit):
handler(_handler),
user_data(_user_data),
base_unit(MOVE(_base_unit)),
run(false),
connecting(false),
retry_time(RETRY_TIME){assert(base_unit.get());}

UnitProxyHandlerWrapper::~UnitProxyHandlerWrapper() {}

int UnitProxyHandlerWrapper::callHandler(unsigned int event, void *event_data) {
    return handler(user_data,
                   event,
                   event_data);
}

int UnitProxyHandlerWrapper::reconnection() {
    int err = 0;
    if((ConnectionStateDisconnected == base_unit->getConnectionState()) &&
       connecting == false) {
        base_unit->connect();
        connecting = true;
    } else {
        connecting = false;
        if((err = callHandler(UP_EV_CONN_RETRY, &retry_time)) == 0) {
            //we need to reset the autorization for new conenction
            base_unit->resetAuthorization();
            sleep(retry_time);
        }
    }
    return err;
}

int UnitProxyHandlerWrapper::eventLoop(int32_t milliseconds_wait) {
    int err = 0;
    //reference to internal conenction state of adapter
    const connection_adapter::ConnectionState& connection_state = base_unit->getConnectionState();
    
    //schedule work
    switch (connection_state) {
        case ConnectionStateConnecting: {
            break;
        }
            
        case ConnectionStateConnected:{
            err = callHandler(UP_EV_CONN_CONNECTED, NULL);
            break;
        }
        case ConnectionStateDisconnected:{
            if((err = callHandler(UP_EV_CONN_DISCONNECTED, NULL)) == 0){
                err = reconnection();
            }
            break;
        }
        case ConnectionStateConnectionError:{
            if((err = callHandler(UP_EV_CONN_ERROR, NULL)) == 0) {
                err = reconnection();
            }
            break;
        }
        case ConnectionStateNotAccepted:{
            if((err = callHandler(UP_EV_CONN_REJECTED, NULL)) == 0){
                err = reconnection();
            }
            break;
        }
        case ConnectionStateAccepted:{
            if(connecting) {
                connecting = false;
                if((err = callHandler(UP_EV_CONN_ACCEPTED, NULL))) {
                    break;
                }
            }
            //give a step to unit handler event loop
            err = unitEventLoop();
            break;
        }
    }
    
    while ( base_unit->hasMoreMessage()) {
        RemoteMessageUniquePtr msg =  base_unit->getNextMessage();
        std::cout << msg->message->toString();
        if(msg->isError()) {
            UPError err_msg = {msg->getErrorCode(),
                msg->getErrorMessage(),
                msg->getErrorDomain()};
            err = callHandler(UP_EV_ERR_RECEIVED, &err_msg);
        }
    }
    
    base_unit->poll(milliseconds_wait);
    
    return err;
}

int UnitProxyHandlerWrapper::poll(int32_t milliseconds_wait) {
    return eventLoop(milliseconds_wait);
}
