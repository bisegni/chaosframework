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

#ifndef __CHAOSFramework_FDFC9046_54C9_4032_B775_70F29A8A8C1C_SearchInstancesForScript_h
#define __CHAOSFramework_FDFC9046_54C9_4032_B775_70F29A8A8C1C_SearchInstancesForScript_h

#include <chaos_service_common/data/script/Script.h>
#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace script {
                
                class SearchInstancesForScript:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(SearchInstancesForScript)
                protected:
                    API_PROXY_CD_DECLARATION(SearchInstancesForScript)
                public:
                    
                    /*!
                     Search into the instances of a script
                     */
                    ApiProxyResult execute(const std::string& script_name,
                                           const std::string& search_string,
                                           const uint64_t last_sequence_id = 0,
                                           const uint32_t page_lenght = 30);
                    
                    static void deserialize(chaos::common::data::CDataWrapper& serialization,
                                            std::vector<chaos::service_common::data::script::ScriptInstance>& instances_found);
                };
                
            }
        }
    }
}

#endif /* __CHAOSFramework_FDFC9046_54C9_4032_B775_70F29A8A8C1C_SearchInstancesForScript_h */
