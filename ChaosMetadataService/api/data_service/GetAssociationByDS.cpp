/*
 *	GetAssociationByDS.cpp
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
    unique_ptr<CDataWrapper> result;
    std::vector<boost::shared_ptr<CDataWrapper> > node_associated;

    GET_DATA_ACCESS(DataServiceDataAccess, ds_da, -3)
    const std::string ds_unique_id = api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);

    if((err = ds_da->searchAssociationForUID(ds_unique_id,
                                            node_associated))){
        LOG_AND_TROW(DS_GET_ASSOC_ERR, err, boost::str(boost::format("Error fetching the association for data service %1%") % ds_unique_id))
    }else{
        result.reset(new CDataWrapper());
        for(std::vector<boost::shared_ptr<CDataWrapper> >::iterator it = node_associated.begin();
            it != node_associated.end();
            it++) {
            result->appendCDataWrapperToArray(*it->get());
        }
        result->finalizeArrayForKey("node_search_result_page");
    }
    return result.release();
}
