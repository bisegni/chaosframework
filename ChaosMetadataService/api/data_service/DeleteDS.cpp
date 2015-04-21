//
//  DeleteDS.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 24/04/15.
//  Copyright (c) 2015 INFN. All rights reserved.
//

#include "DeleteDS.h"

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::data_service;

#define DS_DELETE_DS_INFO INFO_LOG(DeleteDS)
#define DS_DELETE_DS_DBG  DBG_LOG(DeleteDS)
#define DS_DELETE_DS_ERR  ERR_LOG(DeleteDS)

DeleteDS::DeleteDS():
AbstractApi("delete"){

}

DeleteDS::~DeleteDS() {

}

CDataWrapper *DeleteDS::execute(CDataWrapper *api_data,
                                bool& detach_data) throw(chaos::CException) {

    CHECK_CDW_THROW_AND_LOG(api_data, DS_DELETE_DS_ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(api_data, NodeDefinitionKey::NODE_UNIQUE_ID, DS_DELETE_DS_ERR, -2, "The ndk_unique_id key is mandatory")

    int err = 0;
    bool presence = false;
    const std::string ds_uid = api_data->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID);

    GET_DATA_ACCESS(persistence::data_access::DataServiceDataAccess, ds_da, -3)
    if((err = ds_da->checkPresence(ds_uid, presence))) {
        LOG_AND_TROW(DS_DELETE_DS_ERR, err, boost::str(boost::format("Error checking the presence of the data service cuid %1%") % ds_uid));
    }else{
        if(presence) {
            if((err = ds_da->deleteDataService(ds_uid))){
                LOG_AND_TROW(DS_DELETE_DS_ERR, err, boost::str(boost::format("Error deleting the data service:%1%") % ds_uid));
            } else {
                DS_DELETE_DS_DBG << boost::str(boost::format("The data service '%1%' has been deleted") % ds_uid);
            }
        }else{
            LOG_AND_TROW(DS_DELETE_DS_ERR, err, boost::str(boost::format("The data service '%1%' has not been found") % ds_uid));
        }
    }
    return NULL;
}