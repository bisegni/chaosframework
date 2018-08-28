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

#ifndef __CHAOSFramework__E72899A_3BC7_42C8_80C3_E0294CC429D5_SaveScript_h
#define __CHAOSFramework__E72899A_3BC7_42C8_80C3_E0294CC429D5_SaveScript_h

#include <chaos_service_common/data/script/Script.h>
#include <chaos_metadata_service_client/api_proxy/ApiProxy.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace script {
                
                class SaveScript:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(SaveScript)
                protected:
                    API_PROXY_CD_DECLARATION(SaveScript)
                public:
                    /*!
                     Create a new script, the minimal information fro create it
                     are name and description from class @chaos::service_common::data::script::ScriptBaseInfo.
                     \param script_to_insert is a description of a new script
                     */
                    ApiProxyResult execute(const chaos::service_common::data::script::Script& script_to_insert,
                                           const bool& import = false);
                    ApiProxyResult execute( chaos::common::data::CDWUniquePtr script_to_insert,
                                           const bool& import = false);
                };
                
            }
        }
    }
}

#endif /* __CHAOSFramework__E72899A_3BC7_42C8_80C3_E0294CC429D5_SaveScript_h */
