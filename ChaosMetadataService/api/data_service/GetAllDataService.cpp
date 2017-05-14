/*
 *	GetAllDataService.cpp
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

#include "GetAllDataService.h"
#include <boost/format.hpp>
using namespace chaos::common::data;
using namespace chaos::metadata_service::api::data_service;
using namespace chaos::metadata_service::persistence::data_access;

#define DS_GET_ALL_INFO INFO_LOG(GetAllDataService)
#define DS_GET_ALL_DBG  DBG_LOG(GetAllDataService)
#define DS_GET_ALL_ERR  ERR_LOG(GetAllDataService)

GetAllDataService::GetAllDataService():
AbstractApi("getAllDS"){

}

GetAllDataService::~GetAllDataService() {

}

chaos::common::data::CDataWrapper *GetAllDataService::execute(chaos::common::data::CDataWrapper *api_data,
                                                               bool& detach_data) throw(chaos::CException) {

    int err = 0;
    std::unique_ptr<CDataWrapper> result;
    std::vector<boost::shared_ptr<CDataWrapper> > data_services;

    GET_DATA_ACCESS(DataServiceDataAccess, ds_da, -3)

    if((err = ds_da->searchAllDataAccess(data_services, 0, 100))){
        LOG_AND_TROW(DS_GET_ALL_ERR, err, "Error fetching data service list")
    }else{
        result.reset(new CDataWrapper());
        for(std::vector<boost::shared_ptr<CDataWrapper> >::iterator it = data_services.begin();
            it != data_services.end();
            it++) {
            result->appendCDataWrapperToArray(*it->get());
        }
        result->finalizeArrayForKey("node_search_result_page");
    }
    return result.release();
}
