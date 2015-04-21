/*
 *	UpdateDS.cpp
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

#include "UpdateDS.h"

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::data_service;

#define DS_UPDATE_DS_INFO INFO_LOG(UpdateDS)
#define DS_UPDATE_DS_DBG  DBG_LOG(UpdateDS)
#define DS_UPDATE_DS_ERR  ERR_LOG(UpdateDS)

UpdateDS::UpdateDS():
AbstractApi("update"){

}

UpdateDS::~UpdateDS() {

}

CDataWrapper *UpdateDS::execute(CDataWrapper *api_data,
                                bool& detach_data) throw(chaos::CException) {

    CHECK_CDW_THROW_AND_LOG(api_data, DS_UPDATE_DS_ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(api_data, NodeDefinitionKey::NODE_UNIQUE_ID, DS_UPDATE_DS_ERR, -3, "The ndk_unique_id key is mandatory")
    CHECK_KEY_THROW_AND_LOG(api_data, NodeDefinitionKey::NODE_DIRECT_IO_ADDR, DS_UPDATE_DS_ERR, -4, "The ndk_d_io_addr key is mandatory")
    CHECK_KEY_THROW_AND_LOG(api_data, DataServiceNodeDefinitionKey::DS_DIRECT_IO_ENDPOINT, DS_UPDATE_DS_ERR, -5, "The dsndk_direct_io_ep key is mandatory")

    int err = 0;
    bool presence = false;
    const std::string ds_uid = api_data->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID);
    const std::string ds_direct_io_address = api_data->getStringValue(chaos::NodeDefinitionKey::NODE_DIRECT_IO_ADDR);
    const uint32_t ds_direct_io_endpoint = api_data->getUInt32Value(chaos::DataServiceNodeDefinitionKey::DS_DIRECT_IO_ENDPOINT);
    GET_DATA_ACCESS(persistence::data_access::DataServiceDataAccess, ds_da, -4)
    if((err = ds_da->checkPresence(ds_uid, presence))) {
        LOG_AND_TROW(DS_UPDATE_DS_ERR, err, boost::str(boost::format("Error checking the presence of the data service cuid %1%") % ds_uid));
    }else{
        if(presence) {
            if((err = ds_da->updateExisting(ds_uid,
                                            ds_direct_io_address,
                                            ds_direct_io_endpoint))){
                LOG_AND_TROW(DS_UPDATE_DS_ERR, err, boost::str(boost::format("Error creating new data service:%1%") % ds_uid));
            } else {
                DS_UPDATE_DS_DBG << boost::str(boost::format("The data service '%1%' has been updated") % ds_uid);
            }
        }else{
            LOG_AND_TROW(DS_UPDATE_DS_ERR, err, boost::str(boost::format("The data service '%1%' has not been found") % ds_uid));
        }
    }
    return NULL;
}