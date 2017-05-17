/*
 *	AssociationControlUnit.cpp
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

#include "AssociationControlUnit.h"
#include <boost/format.hpp>

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::data_service;
using namespace chaos::metadata_service::persistence::data_access;

#define DS_ASSCIATE_CU_DS_INFO INFO_LOG(AssociationControlUnit)
#define DS_ASSCIATE_CU_DS_DBG  DBG_LOG(AssociationControlUnit)
#define DS_ASSCIATE_CU_DS_ERR  ERR_LOG(AssociationControlUnit)

AssociationControlUnit::AssociationControlUnit():
AbstractApi("associateControlUnit"){

}

AssociationControlUnit::~AssociationControlUnit() {

}

CDataWrapper *AssociationControlUnit::execute(CDataWrapper *api_data,
                                              bool& detach_data) throw(chaos::CException) {

    CHECK_CDW_THROW_AND_LOG(api_data, DS_ASSCIATE_CU_DS_ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(api_data, NodeDefinitionKey::NODE_UNIQUE_ID, DS_ASSCIATE_CU_DS_ERR, -2, "The ndk_unique_id key is mandatory")
    CHECK_KEY_THROW_AND_LOG(api_data, NodeDefinitionKey::NODE_PARENT, DS_ASSCIATE_CU_DS_ERR, -3, "The ndk_parent key is mandatory")
    CHECK_KEY_THROW_AND_LOG(api_data, "associate", DS_ASSCIATE_CU_DS_ERR, -4, "The associate key is mandatory")

    int err = 0;
    bool presence = false;
    ChaosUniquePtr<CMultiTypeDataArrayWrapper> unique_id_vec(api_data->getVectorValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID));
    const std::string ds_uid = api_data->getStringValue(chaos::NodeDefinitionKey::NODE_PARENT);
    const bool associate = api_data->getBoolValue("associate");

    GET_DATA_ACCESS(ControlUnitDataAccess, cu_da, -5)
    GET_DATA_ACCESS(DataServiceDataAccess, ds_da, -6)
    if((err = ds_da->checkPresence(ds_uid, presence))){
        LOG_AND_TROW(DS_ASSCIATE_CU_DS_ERR, err, boost::str(boost::format("Error checking hte data service '%1%' presence")%ds_uid))
    } else {
        if(presence) {
            presence = false;

                //scan all control unit
            for(int idx = 0; idx < unique_id_vec->size(); idx++) {
                    //get unique id and make the chek
                const std::string cu_uid = unique_id_vec->getStringElementAtIndex(idx);
                    //check if it exists
                if((err = cu_da->checkPresence(cu_uid, presence))) {
                    LOG_AND_TROW(DS_ASSCIATE_CU_DS_ERR, err, boost::str(boost::format("Error checking hte control unit '%1%' presence")%cu_uid))
                } else if(presence) {
                        //we can associate
                    if(associate){
                            //associate
                        if((err = ds_da->associateNode(ds_uid, cu_uid))){
                            LOG_AND_TROW(DS_ASSCIATE_CU_DS_ERR, err, boost::str(boost::format("Error associating %1% to the data service %2%")%cu_uid%ds_uid))
                        } else {
                            DS_ASSCIATE_CU_DS_DBG << boost::str(boost::format("%1% has been associate to the data service %2%")%cu_uid%ds_uid);
                        }
                    }else{
                            //remove
                        if((err = ds_da->removeNode(ds_uid, cu_uid))){
                            LOG_AND_TROW(DS_ASSCIATE_CU_DS_ERR, err, boost::str(boost::format("Error removing %1% from the data service %2%")%cu_uid%ds_uid))
                        } else {
                            DS_ASSCIATE_CU_DS_DBG << boost::str(boost::format("%1% has been removed from the data service %2%")%cu_uid%ds_uid);
                        }
                    }
                } else {
                        //no control unit found
                    LOG_AND_TROW(DS_ASSCIATE_CU_DS_ERR, -2, boost::str(boost::format("The control unit '%1%' has not been found")%cu_uid))
                }
            }
        }else {
                //no data service found
            LOG_AND_TROW(DS_ASSCIATE_CU_DS_ERR, -1, boost::str(boost::format("The data service '%1%' has not been found")%ds_uid))
        }
    }

    return NULL;
}
