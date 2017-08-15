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

#ifndef __CHAOSFramework_AC7D1F8B_B75A_4433_958A_5D55485CE572_UpdateScriptOnNode_h
#define __CHAOSFramework_AC7D1F8B_B75A_4433_958A_5D55485CE572_UpdateScriptOnNode_h

#include <chaos_service_common/data/script/Script.h>
#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace script {
                
                //update the node source code on a node tha tis na instance of the script
                class UpdateScriptOnNode:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(UpdateScriptOnNode)
                protected:
                    API_PROXY_CD_DECLARATION(UpdateScriptOnNode)
                public:
                    
                    /*!
                     Create or remove a script instance
                     */
                    ApiProxyResult execute(const std::string& target_node,
                                           const chaos::service_common::data::script::ScriptBaseDescription& sbd);
                    
                    /*!
                     Create or remove multiple script instance
                     */
                    ApiProxyResult execute(const ChaosStringVector& instance_names,
                                           const chaos::service_common::data::script::ScriptBaseDescription& sbd);
                };
                
            }
        }
    }
}

#endif /* __CHAOSFramework_AC7D1F8B_B75A_4433_958A_5D55485CE572_UpdateScriptOnNode_h */
