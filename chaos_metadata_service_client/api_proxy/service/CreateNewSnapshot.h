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

#ifndef __MetadataServiceClient__CreateNewSnapshot_h
#define __MetadataServiceClient__CreateNewSnapshot_h

#include <chaos/common/chaos_types.h>

#include <chaos_metadata_service_client/api_proxy/ApiProxy.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace service {
                
                CHAOS_DEFINE_VECTOR_FOR_TYPE(std::string, SnapshotNodeList)
                
                //! Reset all metadata service backend database
                class CreateNewSnapshot:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(CreateNewSnapshot)
                protected:
                    API_PROXY_CD_DECLARATION(CreateNewSnapshot)
                public:
                    ApiProxyResult execute(const std::string& snapshot_name,
                                           const SnapshotNodeList& node_list);
                };
            }
        }
    }
}

#endif /* CreateNewSnapshot_h */
