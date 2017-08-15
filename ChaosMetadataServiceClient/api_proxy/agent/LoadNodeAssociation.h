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

#ifndef __CHAOSFramework__3A7F499_C69A_46A0_873E_D4C8CAA5E1C5_LoadNodeAssociation_h
#define __CHAOSFramework__3A7F499_C69A_46A0_873E_D4C8CAA5E1C5_LoadNodeAssociation_h

#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

#include <chaos/common/chaos_types.h>

#include <chaos_service_common/data/data.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace agent {
                
                //! load the association information for a not belong to an agent
                class LoadNodeAssociation:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(LoadNodeAssociation)
                protected:
                    API_PROXY_CD_DECLARATION(LoadNodeAssociation)
                public:
                    //! load the association information for a not belong to an agent
                    /*!
                     \param agent_uid the agent unique id
                     \param associated_node_uid the node associated to the agent
                     */
                    ApiProxyResult execute(const std::string& agent_uid,
                                           const std::string& associated_node_uid);
                    
                    static void deserialize(chaos::common::data::CDataWrapper *api_result,
                                            chaos::service_common::data::agent::AgentAssociation& node_association);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__3A7F499_C69A_46A0_873E_D4C8CAA5E1C5_LoadNodeAssociation_h */
