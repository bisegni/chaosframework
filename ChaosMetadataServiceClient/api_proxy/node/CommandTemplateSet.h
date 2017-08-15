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

#ifndef __CHAOSFramework__CommandTemplateSet__
#define __CHAOSFramework__CommandTemplateSet__

#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>
#include <chaos/common/data/CDataWrapperKeyValueSetter.h>

#include <vector>

#include <boost/shared_ptr.hpp>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace node {
                
                //! Set (insert or update) a template for control unit command
                class CommandTemplateSet:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(CommandTemplateSet)
                protected:
                    //! default constructor
                    API_PROXY_CD_DECLARATION(CommandTemplateSet)
                public:
                    //!
                    /*!
                     */
                    ApiProxyResult execute(const TemplateList& template_configuration);
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__CommandTemplateSet__) */
