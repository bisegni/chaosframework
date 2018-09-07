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
void NewUS::performQuery(const std::string& new_us_uid,
                         const string& desc,
                         ChaosUniquePtr<chaos::common::data::CDataWrapper>& custom){
    bool presence = false;
    int err = 0;
    GET_DATA_ACCESS(UnitServerDataAccess, us_da, -3);
    
    if((err = us_da->checkPresence(new_us_uid, presence))) {
        LOG_AND_TROW(US_NEW_ERR, -4, boost::str(boost::format("Error fetching the presence for the uid:%1%") % new_us_uid));
    }
    
    if(presence) {
        LOG_AND_TROW(US_NEW_ERR, -5, boost::str(boost::format("There is already another node with the same uid:%1%") % new_us_uid));
    }
    ChaosUniquePtr<chaos::common::data::CDataWrapper> data_pack(new CDataWrapper());
    data_pack->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, new_us_uid);
    data_pack->addStringValue(NodeDefinitionKey::NODE_TYPE, NodeType::NODE_TYPE_UNIT_SERVER);
    data_pack->addStringValue(NodeDefinitionKey::NODE_DESC, desc);
    if(custom.get()){
        data_pack->addCSDataValue(chaos::NodeDefinitionKey::NODE_CUSTOM_PARAM,*custom);
    }
    if((err = us_da->insertNewUS(*data_pack, false))) {
        LOG_AND_TROW(US_NEW_ERR, -6, boost::str(boost::format("Error creating a new unit server of id:%1%") % new_us_uid));
    }
}

CDWUniquePtr NewUS::execute(CDWUniquePtr api_data) {
    CHECK_CDW_THROW_AND_LOG(api_data, US_NEW_ERR, -1, "No parameter has been set!")
    CHECK_KEY_THROW_AND_LOG(api_data, NodeDefinitionKey::NODE_UNIQUE_ID, US_NEW_ERR, -2, "No mdk_uid is mandatory!")
    
    
    
    //get the parameter
    const std::string new_us_uid = api_data->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID);
    
    const std::string desc = api_data->hasKey(chaos::NodeDefinitionKey::NODE_DESC)?api_data->getStringValue(chaos::NodeDefinitionKey::NODE_DESC):"";
    ChaosUniquePtr<chaos::common::data::CDataWrapper> custom(api_data->hasKey(chaos::NodeDefinitionKey::NODE_CUSTOM_PARAM)?api_data->getCSDataValue(chaos::NodeDefinitionKey::NODE_CUSTOM_PARAM):NULL);
    
    //we can proceed
    performQuery(new_us_uid,desc, custom);
    return NULL;
}
