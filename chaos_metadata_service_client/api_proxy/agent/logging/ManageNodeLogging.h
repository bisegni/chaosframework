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

#ifndef __CHAOSFramework__29AA7C7_A0C5_4E36_9587_E5B0491B8B56_ManageNodeLogging_h
#define __CHAOSFramework__29AA7C7_A0C5_4E36_9587_E5B0491B8B56_ManageNodeLogging_h

#include <chaos_metadata_service_client/api_proxy/ApiProxy.h>

#include <chaos/common/chaos_types.h>

#include <chaos_service_common/data/data.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace agent {
                namespace logging {
                    //! Start a batch process cheecking into the agent
                    class ManageNodeLogging:
                    public chaos::metadata_service_client::api_proxy::ApiProxy {
                        API_PROXY_CLASS(ManageNodeLogging)
                    protected:
                        API_PROXY_CD_DECLARATION(ManageNodeLogging)
                    public:
                        //! Start a batch process cheecking into the agent
                        /*!
                         The metadata server load all associated node and ask to the agent for their
                         alive status
                         \param agent_uid the agent that need to be asked to check his asosciated node
                         */
                        ApiProxyResult execute(const std::string& node_uid,
                                               const chaos::service_common::data::agent::NodeAssociationLoggingOperation logging_operation);
                    };
                }
            }
        }
    }
}

#endif /* __CHAOSFramework__29AA7C7_A0C5_4E36_9587_E5B0491B8B56_ManageNodeLogging_h */
