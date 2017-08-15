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

#ifndef __CHAOSFramework__UpdateProperty__
#define __CHAOSFramework__UpdateProperty__

#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace node {
                
                //! submit an update to a property for a node
                class UpdateProperty:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(UpdateProperty)
                protected:
                    //! default constructor
                    API_PROXY_CD_DECLARATION(UpdateProperty)
                public:
                    /*!
                     \param node_unique_id the unique id of a command
                     \param node_property_groups_list the list of group of property
                     */
                    ApiProxyResult execute(const std::string& node_unique_id,
                                           const NodePropertyGroupList& node_property_groups_list);
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__UpdateProperty__) */
