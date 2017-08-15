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

#ifndef __CHAOSFramework__F805028_2A5B_4E14_AD9E_5B2C6874A4AC_RemoveScript_h
#define __CHAOSFramework__F805028_2A5B_4E14_AD9E_5B2C6874A4AC_RemoveScript_h

#include <chaos_service_common/data/script/Script.h>
#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace script {
                
                class RemoveScript:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(RemoveScript)
                protected:
                    API_PROXY_CD_DECLARATION(RemoveScript)
                public:
                    
                    /*!
                     Create a new script, the minimal information fro create it
                     are name and description from class @chaos::service_common::data::script::ScriptBaseInfo.
                     \param script_to_insert is a description of a new script
                     */
                    ApiProxyResult execute(chaos::service_common::data::script::ScriptBaseDescription& description);
                };
                
            }
        }
    }
}

#endif /* __CHAOSFramework__F805028_2A5B_4E14_AD9E_5B2C6874A4AC_RemoveScript_h */
