/*
 *	GetDomains.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__GetDomains_h
#define __CHAOSFramework__GetDomains_h

#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace groups {
                
                CHAOS_DEFINE_VECTOR_FOR_TYPE(std::string, DomainsList);
                
                class GetDomainsHelper {
                    DomainsList domains_list;
                public:
                    GetDomainsHelper(chaos::common::data::CDataWrapper *api_result);
                    ~GetDomainsHelper();
                    size_t getDomainsListSize();
                    const DomainsList& getDomainsList();
                };
                
                //! Get all domains
                class GetDomains:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(GetDomains)
                protected:
                    API_PROXY_CD_DECLARATION(GetDomains)
                public:
                    
                    ApiProxyResult execute();
                    
                    static ChaosUniquePtr<GetDomainsHelper> getHelper(chaos::common::data::CDataWrapper *api_result);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__GetDomains_h */
