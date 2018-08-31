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

#include "GetDomains.h"

CHAOS_DEFINE_VECTOR_FOR_TYPE(std::string, DomainList)

using namespace chaos::metadata_service::api::groups;

#define G_GD_INFO INFO_LOG(GetDomains)
#define G_GD_DBG  DBG_LOG(GetDomains)
#define G_GD_ERR  ERR_LOG(GetDomains)

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::groups;
using namespace chaos::metadata_service::persistence::data_access;

CHAOS_MDS_DEFINE_API_CD(GetDomains, getDomains)

CDWUniquePtr GetDomains::execute(CDWUniquePtr api_data) {
    int err = 0;
    GET_DATA_ACCESS(TreeGroupDataAccess, tg_da, -1);
    DomainList domain_list;
    if((err = tg_da->getAllGroupDomain(domain_list))){
        LOG_AND_TROW_FORMATTED(G_GD_ERR, -2, "Error returning domain list with error code %1%", %err);
    }
    
    //compose output
    CreateNewDataWrapper(result,);
    if(domain_list.size()) {
        for(DomainListIterator it = domain_list.begin();
            it != domain_list.end();
            it++) {
            result->appendStringToArray(*it);
        }
    }
    result->finalizeArrayForKey("domain_list");
    return result;
}
