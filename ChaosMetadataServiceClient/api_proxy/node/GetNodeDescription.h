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
                protected chaos::metadata_service_client::api_proxy::ApiProxy {
                protected:
                        //! default constructor
                    GetNodeDescription(chaos::common::message::MultiAddressMessageChannel *_mn_message);
                        //! default destructor
                    ~GetNodeDescription();
                public:

                    /*!
                     
                     */
                    void execute(auto_ptr<chaos::common::message::MessageRequestFuture>& result,
                                 const std::string& unique_node_id);
                };

            }
        }
    }
}

#endif /* defined(__CHAOSFramework__GetNodeDescription__) */
