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

#ifndef __CHAOSFramework_E77F92DE_D8B6_4286_9ABF_850CFD93CDD2_UpdateBindType_h
#define __CHAOSFramework_E77F92DE_D8B6_4286_9ABF_850CFD93CDD2_UpdateBindType_h

#include <chaos_service_common/data/script/Script.h>
#include <chaos_metadata_service_client/api_proxy/ApiProxy.h>

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
                    ApiProxyResult execute(const chaos::service_common::data::script::ScriptBaseDescription& sbd,
                                           chaos::service_common::data::script::ScriptInstance& scr_inst);
                    ApiProxyResult execute(const chaos::service_common::data::script::ScriptBaseDescription& sbd,
                                           std::vector<chaos::service_common::data::script::ScriptInstance>& scr_inst_vec);
                };
                
            }
        }
    }
}

#endif /* __CHAOSFramework_E77F92DE_D8B6_4286_9ABF_850CFD93CDD2_UpdateBindType_h */
