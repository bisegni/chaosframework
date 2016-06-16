/*
 *	ManageScriptInstance.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 15/06/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_F6F210A6_CA50_46F8_8F1F_6A71749DB42D_ManageScriptInstance_h
#define __CHAOSFramework_F6F210A6_CA50_46F8_8F1F_6A71749DB42D_ManageScriptInstance_h

#include <chaos_service_common/data/script/Script.h>
#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace script {
                
                class ManageScriptInstance:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(ManageScriptInstance)
                protected:
                    API_PROXY_CD_DECLARATION(ManageScriptInstance)
                public:
                    
                    /*!
                     Create or remove a script instance
                     */
                    ApiProxyResult execute(const std::string& script_name,
                                           const std::string& instance_name,
                                           const bool create);
                    
                    /*!
                     Create or remove multiple script instance
                     */
                    ApiProxyResult execute(const std::string& script_name,
                                           const ChaosStringList& instance_names,
                                           const bool create);
                };
                
            }
        }
    }
}

#endif /* __CHAOSFramework_F6F210A6_CA50_46F8_8F1F_6A71749DB42D_ManageScriptInstance_h */
