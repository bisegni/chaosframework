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

#ifndef __CHAOSFramework_B96F52E0_9934_48BA_BC70_301A02449FF7_ExternalDriverHandlerWrapper_h
#define __CHAOSFramework_B96F52E0_9934_48BA_BC70_301A02449FF7_ExternalDriverHandlerWrapper_h

#include <chaos_micro_unit_toolkit/connection/unit_proxy/UnitProxyHandlerWrapper.h>
#include <chaos_micro_unit_toolkit/connection/unit_proxy/raw_driver/ExternalDriverUnitProxy.h>

#define UP_EV_MSG_RECEIVED      10  /* new message has been received, event_data point to UPMessage*, memory is valido only during call*/
#define UP_EV_REQ_RECEIVED      11  /* new request has been received, event_data point to EDNormalRequest*, memory is valido only during call*/
#define UP_EV_ERR_RECEIVED      12  /* error message has been received, event_data point to UPError*, memory is valid only during call*/
#define UP_EV_INIT_RECEIVED     13  /* has been request a new driver initialization, the event_data point to EDInitResponse*, memory is valid only during call*/
#define UP_EV_DEINIT_RECEIVED   14  /* has been request a new driver deinitilization, the event_data point to EDInitRequest*, memory is valid only during call*/

namespace chaos {
    namespace micro_unit_toolkit {
        namespace connection {
            namespace unit_proxy {
                namespace raw_driver {
                    
                    //!difine an external driver message
                    struct EDMessage {
                        //!driver instance identification
                        const std::string uri;
                        //!driver opcode
                        const std::string opcode;
                        //!opcode paramter
                        data::CDWShrdPtr opcode_par;
                    };
                    
                    //!define an external driver response
                    struct EDResponse {
                        int error_code;
                        std::string error_message;
                        std::string error_domain;
                        
                        EDResponse():
                        error_code(0),
                        error_message(),
                        error_domain(){}
                    };
                    
                    
                    //!Define the aswer for init opcode
                    struct EDInitResponse:
                    public EDResponse {
                        //!is the result of the configuration opration(0-not configured, 1-configured)
                        int32_t configuration_state;
                        //!is the unique id asscoiated to the configuration instances
                        /*!
                         This field must to be used to identified the isntance od the driver., can be composed also suing this
                         alghoritm  (uri/#instance)
                         */
                        std::string new_uri_id;
                        
                        EDInitResponse():
                        configuration_state(0),
                        new_uri_id(){}
                    };
                    
                    struct EDReqResponse:
                    public EDResponse {
                        data::CDWShrdPtr message;
                        EDReqResponse():
                        EDResponse(),
                        message(new data::DataPack()){}
                    };
                    
                    //!identify an external driver request
                    struct EDInitRequest {
                        EDMessage message;
                        EDInitResponse response;
                    };
                    
                    struct EDNormalRequest {
                        EDMessage message;
                        EDReqResponse response;
                    };
                    
                    struct EDDeinitRequest {
                        EDMessage message;
                        EDResponse response;
                    };
                    
                    class ExternalDriverHandlerWrapper:
                    public UnitProxyHandlerWrapper {
                        const UnitState& unit_state;
                        bool authorized = false;
                        data::CDWUniquePtr composeResponseMessage(EDResponse& base_resposne);
                    protected:
                        int unitEventLoop();
                        int manageRemoteMessage();
                    public:
                        ExternalDriverHandlerWrapper(UnitProxyHandler handler,
                                                void *user_data,
                                                ChaosUniquePtr<ExternalDriverUnitProxy>& _u_proxy);
                        ~ExternalDriverHandlerWrapper();
                        
                        //! send spontaneus message to the remote raw driver layer
                        int sendMessage(data::CDWUniquePtr& message_data);
                    };
                    
                }
            }
        }
    }
}

#endif /* __CHAOSFramework_B96F52E0_9934_48BA_BC70_301A02449FF7_ExternalDriverHandlerWrapper_h */
