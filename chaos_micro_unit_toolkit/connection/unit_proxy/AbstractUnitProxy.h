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
#include <chaos_micro_unit_toolkit/connection/protocol_adapter/AbstractProtocolAdapter.h>
namespace chaos {
    namespace micro_unit_toolkit {
        namespace connection {
            namespace unit_proxy {
                //bforward decalration
                class UnitProxyHandlerWrapper;
                
                class RemoteMessage {
                    bool is_error;
                public:
                    data::DataPackSharedPtr message;
                    const bool is_request;
                    const uint32_t message_id;
                    data::DataPackSharedPtr request_message;
                    RemoteMessage(const data::DataPackSharedPtr& _message);
                    
                    bool isError() const;
                    int32_t getErrorCode() const;
                    std::string getErrorMessage() const;
                    std::string getErrorDomain() const;
                };
                
                typedef enum {
                    AuthorizationStateDenied,
                    AuthorizationStateOk,
                    AuthorizationStateRequested,
                    AuthorizationStateUnknown
                } AuthorizationState;
                
                typedef ChaosUniquePtr<RemoteMessage> RemoteMessageUniquePtr;
                
                //! Abstract base class for all unit proxy
                class AbstractUnitProxy {
                    friend class chaos::micro_unit_toolkit::connection::unit_proxy::UnitProxyHandlerWrapper;
                    ChaosUniquePtr<protocol_adapter::AbstractProtocolAdapter> protocol_adapter;
                protected:
                    AuthorizationState authorization_state;
                    virtual int sendMessage(data::DataPackUniquePtr& message_data);
                    
                    bool hasMoreMessage();
                    
                    RemoteMessageUniquePtr getNextMessage();
                    
                    int connect();
                    
                    void poll(int32_t milliseconds_wait = 100);
                    
                    int close();
                public:
                    AbstractUnitProxy(ChaosUniquePtr<protocol_adapter::AbstractProtocolAdapter>& _protocol_adapter);
                    
                    virtual ~AbstractUnitProxy();
                    
                    virtual void authorization(const std::string& authorization_key) = 0;
                    
                    //! need to be called once per connection pool for manage the autorization untile it return true
                    /*!
                     when this function is completed(return true) connection state need to be tested
                     */
                    virtual bool manageAutorizationPhase() = 0;
                    
                    const AuthorizationState& getAuthorizationState() const;
                    
                    const protocol_adapter::ConnectionState& getConnectionState() const;
                    
                    void resetAuthorization();
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__9B333EE_BA6D_4EFC_8445_8AA63946A555_AbstractUnitProxy_h */
