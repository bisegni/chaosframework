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

#ifndef __CHAOSFramework__654AAEE_8861_4C3F_960F_7670F919FCE2_UnitProxyHandlerWrapper_h
#define __CHAOSFramework__654AAEE_8861_4C3F_960F_7670F919FCE2_UnitProxyHandlerWrapper_h

#include <chaos_micro_unit_toolkit/micro_unit_toolkit_types.h>
#include <chaos_micro_unit_toolkit/connection/connection_type.h>
#include <chaos_micro_unit_toolkit/connection/unit_proxy/AbstractUnitProxy.h>
#include <chaos_micro_unit_toolkit/connection/connection_adapter/AbstractConnectionAdapter.h>

#define RETRY_TIME              5

#define UP_EV_USR_ACTION        0   /* user can take here his action event_data is the pointer to the handler warapepr instance that manage the handler function */
#define UP_EV_CONN_CONNECTED    1   /* connection to remote host has been done */
#define UP_EV_CONN_ERROR        3   /* there was and error on connection */
#define UP_EV_CONN_DISCONNECTED 4   /* connection to remote host has been closed */
#define UP_EV_CONN_ACCEPTED     5   /* connection has been accepted */
#define UP_EV_CONN_REJECTED     6   /* connection has been rejexted */
#define UP_EV_CONN_RETRY        7   /* retry to reconnect event_data point to and unsigned integer that is the current waiting time for retry */
#define UP_EV_AUTH_ACCEPTED     8   /* unit has been accepted autorization key*/
#define UP_EV_AUTH_REJECTED     9   /* unit has rejected the autorization key */

namespace chaos {
    namespace micro_unit_toolkit {
        namespace connection {
            namespace unit_proxy {
                
                struct UPError {
                    const int error;
                    const std::string message;
                    const std::string domain;
                };
                
                //!handler pointer that permit to handle the message
                /*
                 the hnder can return error that is returner up to the root poll call
                 */
                typedef int (*UnitProxyHandler)(void *usr_data, unsigned int event, void *event_data);
                
                //Base class use a function as handler for all event
                /*!
                 this is a base class that handle all necessary operation to keep connection and guide user
                 to waht appen on it. All superclass for every unit proxy extends this class to make specifyc uni porxy handler
                 This class try to reconenct to endpoint also if conneciton is reject
                 */
                class UnitProxyHandlerWrapper {
                    UnitProxyHandler handler;
                    void *user_data;
                    bool run;
                    bool connecting;
                    unsigned int retry_time;
                    int reconnection();
                protected:
                    ChaosUniquePtr<AbstractUnitProxy> base_unit;
                    int callHandler(unsigned int event, void *event_data);
                    virtual int eventLoop(int32_t milliseconds_wait);
                    virtual int unitEventLoop() = 0;
                public:
                    UnitProxyHandlerWrapper(UnitProxyHandler _handler,
                                            void *_user_data,
                                            ChaosUniquePtr<AbstractUnitProxy> _base_unit);
                    
                    virtual ~UnitProxyHandlerWrapper();
                    
                    int poll(int32_t milliseconds_wait = 100);
                };
                
            }
        }
    }
}

#endif /* __CHAOSFramework__654AAEE_8861_4C3F_960F_7670F919FCE2_UnitProxyHandlerWrapper_h */
