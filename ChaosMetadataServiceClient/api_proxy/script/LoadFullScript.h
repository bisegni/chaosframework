/*
 *	LoadFullScript.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 30/05/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_CB5EEE12_458C_45E7_82AD_23562A6844ED_LoadFullScript_h
#define __CHAOSFramework_CB5EEE12_458C_45E7_82AD_23562A6844ED_LoadFullScript_h

#include <chaos_service_common/data/script/Script.h>
#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace script {
                
                class LoadFullScript:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(LoadFullScript)
                protected:
                    API_PROXY_CD_DECLARATION(LoadFullScript)
                public:
                    
                    /*!
                     Create a new script, the minimal information fro create it
                     are name and description from class @chaos::service_common::data::script::ScriptBaseInfo.
                     \param sbd the base descripiton of the script theat need to be loaded, needs only the sequence
                     */
                    ApiProxyResult execute(const chaos::service_common::data::script::ScriptBaseDescription& sbd);
                };
                
            }
        }
    }
}

#endif /* __CHAOSFramework_CB5EEE12_458C_45E7_82AD_23562A6844ED_LoadFullScript_h */
