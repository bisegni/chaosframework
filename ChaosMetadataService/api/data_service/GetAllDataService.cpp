//
//  GetAllDataService.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 24/04/15.
//  Copyright (c) 2015 INFN. All rights reserved.
//

#include "GetAllDataService.h"
#include <boost/format.hpp>
using namespace chaos::common::data;
using namespace chaos::metadata_service::api::data_service;
using namespace chaos::metadata_service::persistence::data_access;

#define DS_GET_ALL_INFO INFO_LOG(GetAssociationByDS)
#define DS_GET_ALL_DBG  DBG_LOG(GetAssociationByDS)
#define DS_GET_ALL_ERR  ERR_LOG(GetAssociationByDS)

GetAllDataService::GetAllDataService():
AbstractApi("getAllDS"){

}

GetAllDataService::~GetAllDataService() {

}

chaos::common::data::CDataWrapper *GetAllDataService::execute(chaos::common::data::CDataWrapper *api_data,
                                                               bool& detach_data) throw(chaos::CException) {

    int err = 0;
    auto_ptr<CDataWrapper> result;
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