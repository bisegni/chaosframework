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

#include "AssociationControlUnit.h"
#include <boost/format.hpp>

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::data_service;
using namespace chaos::metadata_service::persistence::data_access;

#define DS_ASSCIATE_CU_DS_INFO INFO_LOG(AssociationControlUnit)
#define DS_ASSCIATE_CU_DS_DBG  DBG_LOG(AssociationControlUnit)
#define DS_ASSCIATE_CU_DS_ERR  ERR_LOG(AssociationControlUnit)

CHAOS_MDS_DEFINE_API_CD(AssociationControlUnit, associateControlUnit)

CDWUniquePtr AssociationControlUnit::execute(CDWUniquePtr api_data){

    CHECK_CDW_THROW_AND_LOG(api_data, DS_ASSCIATE_CU_DS_ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(api_data, NodeDefinitionKey::NODE_UNIQUE_ID, DS_ASSCIATE_CU_DS_ERR, -2, "The ndk_unique_id key is mandatory")
    CHECK_KEY_THROW_AND_LOG(api_data, NodeDefinitionKey::NODE_PARENT, DS_ASSCIATE_CU_DS_ERR, -3, "The ndk_parent key is mandatory")
    CHECK_KEY_THROW_AND_LOG(api_data, "associate", DS_ASSCIATE_CU_DS_ERR, -4, "The associate key is mandatory")

    int err = 0;
    bool presence = false;
    CMultiTypeDataArrayWrapperSPtr unique_id_vec(api_data->getVectorValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID));
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
    return CDWUniquePtr();
}
