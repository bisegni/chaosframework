/*
 *	GetLogTypesForSourceUID.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 17/02/16 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
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
                    //! Get all log types
                    /*!
                     return all emitted logs types by a source uid
                     \param source node that has emitted the log types
                     */
                    ApiProxyResult execute(const std::string& source);
                    
                    
                    static std::auto_ptr<GetLogDomainForSourceUIDHelper> getHelper(chaos::common::data::CDataWrapper *api_result);
                };
                
            }
        }
    }
}

#endif /* __CHAOSFramework__GetLogDomainForSourceUID_h */
