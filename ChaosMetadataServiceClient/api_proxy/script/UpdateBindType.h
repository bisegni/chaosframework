/*
 *	UpdateBindType.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 23/05/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_E77F92DE_D8B6_4286_9ABF_850CFD93CDD2_UpdateBindType_h
#define __CHAOSFramework_E77F92DE_D8B6_4286_9ABF_850CFD93CDD2_UpdateBindType_h

#include <chaos_service_common/data/script/Script.h>
#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace script {
                
                //update the node source code on a node tha tis na instance of the script
                class UpdateBindType:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(UpdateBindType)
                protected:
                    API_PROXY_CD_DECLARATION(UpdateBindType)
                public:
                    
                    /*!
                     Create or remove a script instance
                     */
                    ApiProxyResult execute(const chaos::service_common::data::script::ScriptBaseDescription& sbd,
                                           chaos::service_common::data::script::ScriptInstance& scr_inst);
                };
                
            }
        }
    }
}

#endif /* __CHAOSFramework_E77F92DE_D8B6_4286_9ABF_850CFD93CDD2_UpdateBindType_h */
