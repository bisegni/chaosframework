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
