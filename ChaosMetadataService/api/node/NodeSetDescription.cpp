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

#include "NodeSetDescription.h"

#define USRA_INFO INFO_LOG(NodeSetDescription)
#define USRA_DBG  DBG_LOG(NodeSetDescription)
#define USRA_ERR  ERR_LOG(NodeSetDescription)

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::node;
using namespace chaos::metadata_service::persistence::data_access;

CHAOS_MDS_DEFINE_API_CLASS_CD(NodeSetDescription, "setNodeDescription")

CDWUniquePtr NodeSetDescription::execute(CDWUniquePtr api_data) {
    int err = 0;
    bool presence = false;
    if(!api_data->hasKey(NodeDefinitionKey::NODE_UNIQUE_ID)) {
        LOG_AND_TROW(USRA_ERR, -1, "Node unique id not set as parameter")
    }
    
    //!get the unit server data access
    GET_DATA_ACCESS(NodeDataAccess, n_da, -2)
    if((err = n_da->checkNodePresence(presence,
                                      api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID)))) {
        LOG_AND_TROW(USRA_ERR, err, "Error checking node presence")
    } else if(presence){
        if((err = n_da->setNodeDescription(api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID), api_data))) {
            LOG_AND_TROW(USRA_ERR, err, "Error Setting node  '"+   api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID)+"' decription")
        }
    } else {
        LOG_AND_TROW(USRA_ERR, -3, "Node '"+   api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID)+"' not found")
    }
    return CDWUniquePtr();
}
