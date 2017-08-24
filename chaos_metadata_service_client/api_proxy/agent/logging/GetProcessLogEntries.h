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

#ifndef __CHAOSFramework__7A969D3_B3CE_4D31_B393_80AE958E2055_GetProcessLogEntries_h
#define __CHAOSFramework__7A969D3_B3CE_4D31_B393_80AE958E2055_GetProcessLogEntries_h

#include <chaos_metadata_service_client/api_proxy/ApiProxy.h>

#include <chaos/common/chaos_types.h>

#include <chaos_service_common/data/data.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace agent {
                namespace logging {
                    //! Get some entries of the process identified by the id
                    class GetProcessLogEntries:
                    public chaos::metadata_service_client::api_proxy::ApiProxy {
                        API_PROXY_CLASS(GetProcessLogEntries)
                    protected:
                        API_PROXY_CD_DECLARATION(GetProcessLogEntries)
                    public:
                        ApiProxyResult execute(const std::string& node_uid,
                                               const int32_t number_of_entries,
                                               const bool asc,
                                               const uint64_t start_seq);
                        void deserialize(chaos::common::data::CDataWrapper *api_result,
                                         chaos::service_common::data::agent::VectorAgentLogEntry& log_entries_vec);
                    };
                }
            }
        }
    }
}

#endif /* __CHAOSFramework__7A969D3_B3CE_4D31_B393_80AE958E2055_GetProcessLogEntries_h */
