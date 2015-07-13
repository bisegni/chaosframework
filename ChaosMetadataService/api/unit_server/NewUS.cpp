/*
 *	NewUS.cpp
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

#include "NewUS.h"

#include <boost/format.hpp>

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::unit_server;
using namespace chaos::metadata_service::persistence::data_access;

#define US_NEW_INFO INFO_LOG(NewUS)
#define US_NEW_DBG  DBG_LOG(NewUS)
#define US_NEW_ERR  ERR_LOG(NewUS)

NewUS::NewUS():
AbstractApi("newUS"){

}

NewUS::~NewUS() {

}

CDataWrapper *NewUS::execute(CDataWrapper *api_data,
                                             bool& detach_data) throw(chaos::CException) {

    CHECK_CDW_THROW_AND_LOG(api_data, US_NEW_ERR, -1, "No parameter has been set!")
    CHECK_KEY_THROW_AND_LOG(api_data, NodeDefinitionKey::NODE_UNIQUE_ID, US_NEW_ERR, -2, "No mdk_uid is mandatory!")
    
    int err = 0;
    bool presence = false;
    GET_DATA_ACCESS(UnitServerDataAccess, us_da, -3)
        //get the parameter
    const std::string new_us_uid = api_data->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID);
    if((err = us_da->checkPresence(new_us_uid, presence))) {
        LOG_AND_TROW(US_NEW_ERR, -4, boost::str(boost::format("Error fetchi the presence for the uid:%1%") % new_us_uid));
    }

    if(presence) {
        LOG_AND_TROW(US_NEW_ERR, -5, boost::str(boost::format("There is already another node with the same uid:%1%") % new_us_uid));
    }

    std::auto_ptr<CDataWrapper> data_pack(new CDataWrapper());
    data_pack->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, new_us_uid);
    data_pack->addStringValue(NodeDefinitionKey::NODE_TYPE, NodeType::NODE_TYPE_UNIT_SERVER);
        //we can proceed
    if((err = us_da->insertNewUS(*data_pack.get(), false))) {
        LOG_AND_TROW(US_NEW_ERR, -6, boost::str(boost::format("Error creating a new unit server of id:%1%") % new_us_uid));
    }
    return NULL;
}