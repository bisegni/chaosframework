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
#ifndef __CHAOSFramework__CommandTemplateSubmit__
#define __CHAOSFramework__CommandTemplateSubmit__

#include <chaos_metadata_service_client/api_proxy/ApiProxy.h>

#include <string>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace node {
                
                //! Search the template
                class CommandTemplateSubmit:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(CommandTemplateSubmit)
                protected:
                    //! default constructor
                    API_PROXY_CD_DECLARATION(CommandTemplateSubmit)
                public:
                    //!return a page for a template search filtering on command uid
                    /*!
                     \param uid_list a stirng list that contains the uid to include into the search
                     */
                    ApiProxyResult execute(const TemplateSubmissionList& submission_task_list);
                    ApiProxyResult execute(common::data::CDWUniquePtr& command);
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__CommandTemplateSubmit__) */
