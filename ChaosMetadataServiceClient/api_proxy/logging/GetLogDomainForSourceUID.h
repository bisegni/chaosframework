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

#ifndef __CHAOSFramework__GetLogDomainForSourceUID_h
#define __CHAOSFramework__GetLogDomainForSourceUID_h

#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>
#include <ChaosMetadataServiceClient/api_proxy/logging/logging_types.h>

#include <chaos/common/chaos_types.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace logging {
                
                //helper of the api
                class GetLogDomainForSourceUIDHelper {
                    LogDomainList log_type_list;
                public:
                    GetLogDomainForSourceUIDHelper(chaos::common::data::CDataWrapper *api_result);
                    ~GetLogDomainForSourceUIDHelper();
                    size_t getLogDomainListSize();
                    const LogDomainList& getLogDomainList();
                };
                
                //! get log entries for a source
                class GetLogDomainForSourceUID:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(GetLogDomainForSourceUID)
                protected:
                    API_PROXY_CD_DECLARATION(GetLogDomainForSourceUID)
                public:
                    //! Get all log types for all source
                    /*!
                     return all emitted logs types by aall sources
                     */
                    ApiProxyResult execute();
                    
                    //! Get all log types for a single source
                    /*!
                     return all emitted logs types by a source uid
                     \param source node that has emitted the log types
                     */
                    ApiProxyResult execute(const std::string& source);
                    
                    //! Get all log types for multiple source
                    /*!
                     return all emitted logs types by multiple source uid
                     \param source nodes that has emitted the log types
                     */
                    ApiProxyResult execute(const std::vector<std::string>& source);
                    
                    static ChaosUniquePtr<GetLogDomainForSourceUIDHelper> getHelper(chaos::common::data::CDataWrapper *api_result);
                };
                
            }
        }
    }
}

#endif /* __CHAOSFramework__GetLogDomainForSourceUID_h */
