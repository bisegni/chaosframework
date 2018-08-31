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

#include "GetAllDataService.h"
#include <boost/format.hpp>
using namespace chaos::common::data;
using namespace chaos::metadata_service::api::data_service;
using namespace chaos::metadata_service::persistence::data_access;

#define DS_GET_ALL_INFO INFO_LOG(GetAllDataService)
#define DS_GET_ALL_DBG  DBG_LOG(GetAllDataService)
#define DS_GET_ALL_ERR  ERR_LOG(GetAllDataService)

CHAOS_MDS_DEFINE_API_CD(GetAllDataService, getAllDS)

CDWUniquePtr GetAllDataService::execute(CDWUniquePtr api_data) {
    int err = 0;
    CDWUniquePtr result;
    std::vector<ChaosSharedPtr<CDataWrapper> > data_services;

    GET_DATA_ACCESS(DataServiceDataAccess, ds_da, -3)

    if((err = ds_da->searchAllDataAccess(data_services, 0, 100))){
        LOG_AND_TROW(DS_GET_ALL_ERR, err, "Error fetching data service list")
    }else{
        result.reset(new CDataWrapper());
        for(std::vector<ChaosSharedPtr<CDataWrapper> >::iterator it = data_services.begin();
            it != data_services.end();
            it++) {
            result->appendCDataWrapperToArray(*it->get());
        }
        result->finalizeArrayForKey("node_search_result_page");
    }
    return result;
}
