/*
 *	SearchScript.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 25/05/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__D12F532_496B_406D_8B49_AB2851CCFDD0_SearchScript_h
#define __CHAOSFramework__D12F532_496B_406D_8B49_AB2851CCFDD0_SearchScript_h

#include <chaos_service_common/data/script/Script.h>
#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace script {
                
                class SearchScript:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(SearchScript)
                protected:
                    API_PROXY_CD_DECLARATION(SearchScript)
                public:
                    
                    /*!
                     Create a new script, the minimal information fro create it
                     are name and description from class @chaos::service_common::data::script::ScriptBaseInfo.
                     \param script_to_insert is a description of a new script
                     */
                    ApiProxyResult execute(const std::string& search_string,
                                           const uint64_t last_sequence_id = 0,
                                           const uint32_t page_lenght = 30);
                };
                
            }
        }
    }
}

#endif /* __CHAOSFramework__D12F532_496B_406D_8B49_AB2851CCFDD0_SearchScript_h */
