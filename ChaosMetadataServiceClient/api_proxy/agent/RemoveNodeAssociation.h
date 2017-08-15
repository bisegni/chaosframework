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

#ifndef __CHAOSFramework__4AF933B_8306_4117_8A6E_1835E700C1E5_RemoveNodeAssociation_h
#define __CHAOSFramework__4AF933B_8306_4117_8A6E_1835E700C1E5_RemoveNodeAssociation_h

#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

#include <chaos/common/chaos_types.h>

#include <chaos_service_common/data/data.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace agent {
                
                //! remove the association information for a node that belong to an agent
                class RemoveNodeAssociation:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(RemoveNodeAssociation)
                protected:
                    API_PROXY_CD_DECLARATION(RemoveNodeAssociation)
                public:
                    //! remove the association information for a node that belong to an agent
                    /*!
                     \param agent_uid the agent unique id
                     \param associated_node_uid the nodeto remove from the agent
                     */
                    ApiProxyResult execute(const std::string& agent_uid,
                                           const std::string& associated_node_uid);
                    //! remove multiple association information that belong to an agent
                    /*!
                     \param agent_uid the agent unique id
                     \param associated_node_uid a vector of node associated to the agent
                     */
                    ApiProxyResult execute(const std::string& agent_uid,
                                           ChaosStringVector& associated_node_uid);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__4AF933B_8306_4117_8A6E_1835E700C1E5_RemoveNodeAssociation_h */
