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

#ifndef __CHAOSFramework_B6736975_C5CA_45F4_A7AD_A9B95F8DCB32_SaveNodeAssociation_h
#define __CHAOSFramework_B6736975_C5CA_45F4_A7AD_A9B95F8DCB32_SaveNodeAssociation_h

#include <chaos_metadata_service_client/api_proxy/ApiProxy.h>

#include <chaos/common/chaos_types.h>

#include <chaos_service_common/data/data.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace agent {
                
                //! save the association information fora not into an agent
                class SaveNodeAssociation:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(SaveNodeAssociation)
                protected:
                    API_PROXY_CD_DECLARATION(SaveNodeAssociation)
                public:
                    //! Save a node association into an agent
                    /*!
                     \param agent_uid the agent unique id
                     \param node_association the node assocaition description
                     */
                    ApiProxyResult execute(const std::string& agent_uid,
                                           chaos::service_common::data::agent::AgentAssociation& node_association);
                    
                    ApiProxyResult execute(const std::string& agent_uid,
                                                              chaos::common::data::CDataWrapper& node_association);

                    //! Save multiple node association into an agent
                    /*!
                     \param agent_uid the agent unique id
                     \param multiple_node_association a vector of node assocaition description
                     */
                    ApiProxyResult execute(const std::string& agent_uid,
                                           chaos::service_common::data::agent::VectorAgentAssociation& multiple_node_association);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework_B6736975_C5CA_45F4_A7AD_A9B95F8DCB32_SaveNodeAssociation_h */
