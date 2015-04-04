//
//  GetNodeDescription.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 27/03/15.
//  Copyright (c) 2015 INFN. All rights reserved.
//

#ifndef __CHAOSFramework__GetNodeDescription__
#define __CHAOSFramework__GetNodeDescription__

#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace node {

                class GetNodeDescription:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(GetNodeDescription)
                protected:
                    API_PROXY_CD_DECLARATION(GetNodeDescription)
                public:

                    /*!
                     Return the description of the node
                     \param unique_node_id unique id of the node for which we need the colpete description
                     */
                    ApiProxyResult execute(const std::string& unique_node_id);
                };

            }
        }
    }
}

#endif /* defined(__CHAOSFramework__GetNodeDescription__) */
