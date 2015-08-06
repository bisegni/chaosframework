/*
 *	UpdateProperty.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
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
