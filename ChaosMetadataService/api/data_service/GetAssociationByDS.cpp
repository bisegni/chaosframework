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

#include "GetAssociationByDS.h"
#include <boost/format.hpp>
using namespace chaos::common::data;
using namespace chaos::metadata_service::api::data_service;
using namespace chaos::metadata_service::persistence::data_access;

#define DS_GET_ASSOC_INFO INFO_LOG(GetAssociationByDS)
#define DS_GET_ASSOC_DBG  DBG_LOG(GetAssociationByDS)
#define DS_GET_ASSOC_ERR  ERR_LOG(GetAssociationByDS)

GetAssociationByDS::GetAssociationByDS():
AbstractApi("getAssociation"){
    
}

GetAssociationByDS::~GetAssociationByDS() {
    
}

chaos::common::data::CDataWrapper *GetAssociationByDS::execute(chaos::common::data::CDataWrapper *api_data,
                                                               bool& detach_data) throw(chaos::CException) {
    CHECK_CDW_THROW_AND_LOG(api_data,
                            DS_GET_ASSOC_ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(api_data,
                            NodeDefinitionKey::NODE_UNIQUE_ID,
                            DS_GET_ASSOC_ERR, -2, "The ndk_unique_id key is mandatory")
    
    int err = 0;
    std::vector<ChaosSharedPtr<CDataWrapper> > node_associated;
    
    GET_DATA_ACCESS(DataServiceDataAccess, ds_da, -3)
    const std::string ds_unique_id = api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    
    if((err = ds_da->searchAssociationForUID(ds_unique_id,
                                             node_associated))){
        LOG_AND_TROW(DS_GET_ASSOC_ERR, err, boost::str(boost::format("Error fetching the association for data service %1%") % ds_unique_id))
    }
    
    ChaosUniquePtr<chaos::common::data::CDataWrapper>  result(new CDataWrapper());
    for(std::vector<ChaosSharedPtr<CDataWrapper> >::iterator it = node_associated.begin();
        it != node_associated.end();
        it++) {
        result->appendCDataWrapperToArray(*it->get());
    }
    result->finalizeArrayForKey("node_search_result_page");
    return result.release();
}
