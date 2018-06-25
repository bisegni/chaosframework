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

#ifndef __CHAOSFramework__9B333EE_BA6D_4EFC_8445_8AA63946A555_AbstractUnitProxy_h
#define __CHAOSFramework__9B333EE_BA6D_4EFC_8445_8AA63946A555_AbstractUnitProxy_h

#include <chaos_micro_unit_toolkit/data/DataPack.h>
#include <chaos_micro_unit_toolkit/micro_unit_toolkit_types.h>
#include <chaos_micro_unit_toolkit/connection/connection_adapter/AbstractConnectionAdapter.h>

#define AUTHORIZATION_KEY       "authorization_key"
#define AUTHORIZATION_STATE     "authorization_state"
#define REQUEST_KEY             "req_id"
#define MSG_KEY                 "msg"
#define ERR_CODE                "err"
#define ERR_MSG                 "err_msg"
#define ERR_DOM                 "err_dmn"

namespace chaos {
    namespace micro_unit_toolkit {
        namespace connection {
            namespace unit_proxy {
                //bforward decalration
                class UnitProxyHandlerWrapper;
                
                //!decode a message received from remote andpoint
                class RemoteMessage {
                    bool is_error;
                public:
                    data::CDWShrdPtr remote_message;
                    const bool is_request;
                    const uint32_t message_id;
                    data::CDWShrdPtr message;
                    RemoteMessage(const data::CDWShrdPtr& _remote_message);
                    
                    bool isError() const;
                    int32_t getErrorCode() const;
                    std::string getErrorMessage() const;
                    std::string getErrorDomain() const;
                };
                
                typedef enum {
                    UnitStateUnknown,
                    UnitStateNotAuthenticated,
                    UnitStateAuthenticated
                } UnitState;
                
                typedef ChaosUniquePtr<RemoteMessage> RemoteMessageUniquePtr;
                
                //! Abstract base class for all unit proxy
                class AbstractUnitProxy {
                    friend class chaos::micro_unit_toolkit::connection::unit_proxy::UnitProxyHandlerWrapper;
                    ChaosUniquePtr<connection_adapter::AbstractConnectionAdapter> connection_adapter;
                protected:
                    UnitState unit_state;
                    const std::string authorization_key;
                    virtual int sendMessage(data::CDWUniquePtr& message_data);
                    
                    bool hasMoreMessage();
                    
                    RemoteMessageUniquePtr getNextMessage();
                    
                    int connect();
                    
                    void poll(int32_t milliseconds_wait = 100);
                    
                    int close();

                    void manageAuthenticationRequest();
                public:
                    AbstractUnitProxy(const std::string& _authorization_key,
                                      ChaosUniquePtr<connection_adapter::AbstractConnectionAdapter>& _protocol_adapter);
                    
                    virtual ~AbstractUnitProxy();
                    
                    const UnitState& getUnitState() const;
                    
                    const connection_adapter::ConnectionState& getConnectionState() const;
                    
                    void resetAuthorization();
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__9B333EE_BA6D_4EFC_8445_8AA63946A555_AbstractUnitProxy_h */
