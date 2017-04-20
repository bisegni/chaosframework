/*
 *	UpdateScriptOnNode.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 20/04/2017 INFN, National Institute of Nuclear Physics
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
