/*
 * Copyright 2012, 05/02/2018 INFN
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

#ifndef __CHAOSFramework__ForwardNodeRpcMessage__
#define __CHAOSFramework__ForwardNodeRpcMessage__

#include "../AbstractApi.h"
namespace chaos {
    namespace metadata_service {
        namespace api {
            namespace node {
                
                //! Permit to forward an rpc action to a specified node
                /*
                 The api permit to send to a remote node an rpc action message
                 the mandatory key are:
                 NodeDefinitionKey::NODE_UNIQUE_ID
                 RpcActionDefinitionKey::CS_CMDM_ACTION_NAME
                 
                 The message key RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE is optional
                 */
                class ForwardNodeRpcMessage:
                public AbstractApi  {
                public:
                    ForwardNodeRpcMessage();
                    ForwardNodeRpcMessage(const std::string& superclass_api_name);
                    ~ForwardNodeRpcMessage();
                    chaos::common::data::CDWUniquePtr execute(chaos::common::data::CDWUniquePtr api_data);
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__ForwardNodeRpcMessage__) */
