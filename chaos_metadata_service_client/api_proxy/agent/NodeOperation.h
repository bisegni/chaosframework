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

#ifndef __CHAOSFramework_B4233352_30A8_42F0_84AE_39941D11E93D_NodeOperation_h
#define __CHAOSFramework_B4233352_30A8_42F0_84AE_39941D11E93D_NodeOperation_h

#include <chaos_metadata_service_client/api_proxy/ApiProxy.h>

#include <chaos/common/chaos_types.h>

#include <chaos_service_common/data/data.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace agent {
                
                //! remove the association information for a node that belong to an agent
                class NodeOperation:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(NodeOperation)
                protected:
                    API_PROXY_CD_DECLARATION(NodeOperation)
                public:
                    //! Start a node usign an agent
                    /*!
                     The mds automatically found the agent that contains the node and send request to that agent to start it
                     \param node_uid the node unique id to start
                     */
                    ApiProxyResult execute(const std::string& node_uid,
                                           chaos::service_common::data::agent::NodeAssociationOperation op);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework_B4233352_30A8_42F0_84AE_39941D11E93D_NodeOperation_h */
