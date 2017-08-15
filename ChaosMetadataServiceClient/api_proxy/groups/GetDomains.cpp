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

ChaosUniquePtr<GetDomainsHelper> GetDomains::getHelper(CDataWrapper *api_result) {
    return ChaosUniquePtr<GetDomainsHelper>(new GetDomainsHelper(api_result));
}


GetDomainsHelper::GetDomainsHelper(CDataWrapper *api_result) {
    //now we have the result
    //SnapshotInformationPtr
    if(!api_result || !api_result->hasKey("domain_list")) return;
    
    ChaosUniquePtr<CMultiTypeDataArrayWrapper> domains_vec(api_result->getVectorValue("domain_list"));
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
