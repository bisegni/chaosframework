/*
 *	GetAllDomains.cpp
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

#include <ChaosMetadataServiceClient/api_proxy/groups/GetDomains.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::groups;

API_PROXY_CD_DEFINITION(GetDomains,
                        "groups",
                        "getDomains")
/*!
 
 */
ApiProxyResult GetDomains::execute() {
    return callApi();
}

std::auto_ptr<GetDomainsHelper> GetDomains::getHelper(CDataWrapper *api_result) {
    return std::auto_ptr<GetDomainsHelper>(new GetDomainsHelper(api_result));
}


GetDomainsHelper::GetDomainsHelper(CDataWrapper *api_result) {
    //now we have the result
    //SnapshotInformationPtr
    if(!api_result || !api_result->hasKey("domain_list")) return;
    
    std::auto_ptr<CMultiTypeDataArrayWrapper> domains_vec(api_result->getVectorValue("domain_list"));
    for(int idx = 0;
        idx < domains_vec->size();
        idx++) {
        const std::string domain = domains_vec->getStringElementAtIndex(idx);
        domains_list.push_back(domain);
    }
}

GetDomainsHelper::~GetDomainsHelper() {}

size_t GetDomainsHelper::getDomainsListSize() {
    return domains_list.size();
}
const DomainsList& GetDomainsHelper::getDomainsList(){
    return domains_list;
}
