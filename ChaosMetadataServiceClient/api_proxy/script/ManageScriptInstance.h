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
                    ApiProxyResult execute(const uint64_t script_seq,
                                           const std::string& script_name,
                                           const std::string& instance_name,
                                           const bool create);
                    
                    /*!
                     Create or remove multiple script instance
                     */
                    ApiProxyResult execute(const uint64_t script_seq,
                                           const std::string& script_name,
                                           const ChaosStringVector& instance_names,
                                           const bool create);
                };
                
            }
        }
    }
}

#endif /* __CHAOSFramework_F6F210A6_CA50_46F8_8F1F_6A71749DB42D_ManageScriptInstance_h */
