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

#ifndef __CHAOSFramework__GetCommandAndTemplateDescription__
#define __CHAOSFramework__GetCommandAndTemplateDescription__

#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

#include <string>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace node {
                
                //! Return the complete description fo the tempalte and relative command
                class GetCommandAndTemplateDescription:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(GetCommandAndTemplateDescription)
                protected:
                    //! default constructor
                    API_PROXY_CD_DECLARATION(GetCommandAndTemplateDescription)
                public:
                    /*!
                     \param template_name the name of template
                     \param command_uid the command unique id
                     */
                    ApiProxyResult execute(const std::string& template_name,
                                           const std::string& command_uid);
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__GetCommandAndTemplateDescription__) */
