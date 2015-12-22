/*
 *	GetDomains.cpp
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2016 INFN, National Institute of Nuclear Physics
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

#include "GetDomains.h"

CHAOS_DEFINE_VECTOR_FOR_TYPE(std::string, DomainList)

using namespace chaos::metadata_service::api::groups;

#define G_GD_INFO INFO_LOG(GetDomains)
#define G_GD_DBG  DBG_LOG(GetDomains)
#define G_GD_ERR  ERR_LOG(GetDomains)

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::groups;
using namespace chaos::metadata_service::persistence::data_access;

GetDomains::GetDomains():
AbstractApi("getDomains"){
    
}

GetDomains::~GetDomains() {
    
}

chaos::common::data::CDataWrapper *GetDomains::execute(chaos::common::data::CDataWrapper *api_data, bool& detach_data) {
    int err = 0;

    GET_DATA_ACCESS(TreeGroupDataAccess, tg_da, -1);
    DomainList domain_list;
    if((err = tg_da->getAllGroupDomain(domain_list))){
        LOG_AND_TROW_FORMATTED(G_GD_ERR, -2, "Error returning domain list with error code %1%", %err);
    }
    
    //compose output
    std::auto_ptr<CDataWrapper> result(new CDataWrapper());
    if(domain_list.size()) {
        for(DomainListIterator it = domain_list.begin();
            it != domain_list.end();
            it++) {
            result->appendStringToArray(*it);
        }
    }
    result->finalizeArrayForKey("domain_list");
    return NULL;
}