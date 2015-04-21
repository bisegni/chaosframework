/*
 *	GetDescription.cpp
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyrigh 2015 INFN, National Institute of Nuclear Physics
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
#include "GetDescription.h"

#define US_GD_INFO INFO_LOG(GetDescription)
#define US_GD_DBG  DBG_LOG(GetDescription)
#define US_GD_ERR  ERR_LOG(GetDescription)

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::unit_server;

GetDescription::GetDescription():
AbstractApi("getDescription"){

}

GetDescription::~GetDescription() {

}

chaos::common::data::CDataWrapper *GetDescription::execute(chaos::common::data::CDataWrapper *api_data, bool& detach_data) {
    int err = 0;
    bool presence = false;
    chaos::common::data::CDataWrapper *result = NULL;
    if(!api_data->hasKey(NodeDefinitionKey::NODE_UNIQUE_ID)) {
        LOG_AND_TROW(US_GD_ERR, -1, "Node unique id not set as parameter")
    }

        //!get the unit server data access
    persistence::data_access::UnitServerDataAccess *us_da = getPersistenceDriver()->getDataAccess<persistence::data_access::UnitServerDataAccess>();
    if((err = us_da->checkPresence(api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID),
                                   presence))) {
        LOG_AND_TROW(US_GD_ERR, err, "Error checking node presence")
    } else if(presence){
        if((err = us_da->getDescription(api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID), &result))) {
            LOG_AND_TROW(US_GD_ERR, err, "Error fetching unit server decription")
        }
    } else {
        LOG_AND_TROW(US_GD_ERR, -2, "Node not found")
    }
    return result;
    
}