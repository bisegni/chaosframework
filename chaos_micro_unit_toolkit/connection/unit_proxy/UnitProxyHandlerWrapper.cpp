/*
 *	UnitProxyHandlerWrapper.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 10/08/2017 INFN, National Institute of Nuclear Physics
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

#include <chaos_micro_unit_toolkit/connection/unit_proxy/UnitProxyHandlerWrapper.h>
#include <iostream>
#include <unistd.h>
#include <cassert>

using namespace chaos::micro_unit_toolkit::data;
using namespace chaos::micro_unit_toolkit::connection::unit_proxy;
using namespace chaos::micro_unit_toolkit::connection::protocol_adapter;

UnitProxyHandlerWrapper::UnitProxyHandlerWrapper(UnitProxyHandler _handler,
                                                 void *_user_data,
                                                 ChaosUniquePtr<AbstractUnitProxy> _base_unit):
handler(_handler),
user_data(_user_data),
base_unit(ChaosMoveOperator(_base_unit)),
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
    const protocol_adapter::ConnectionState& connection_state = base_unit->getConnectionState();
    
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
