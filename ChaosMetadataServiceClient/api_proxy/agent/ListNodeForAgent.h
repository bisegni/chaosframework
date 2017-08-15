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

#ifndef __CHAOSFramework__0BBEA50_92B1_4D03_88AF_F5FD9AEE27FA_ListNodeForAgent_h
#define __CHAOSFramework__0BBEA50_92B1_4D03_88AF_F5FD9AEE27FA_ListNodeForAgent_h

#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

#include <chaos/common/chaos_types.h>

#include <chaos_service_common/data/node/Agent.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace agent {
                
                //! return all node associated to an agent
                class ListNodeForAgent:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(ListNodeForAgent)
                protected:
                    API_PROXY_CD_DECLARATION(ListNodeForAgent)
                public:
                    //! find all node associated to the agent
                    /*!
                     \param agent_uid the agent unique id
                     */
                    ApiProxyResult execute(const std::string& agent_uid);
                    
                    static void deserialize(chaos::common::data::CDataWrapper *api_result,
                                            chaos::service_common::data::agent::VectorAgentAssociationStatus& associated_nodes);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__0BBEA50_92B1_4D03_88AF_F5FD9AEE27FA_ListNodeForAgent_h */
