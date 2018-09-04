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

#include "DeleteUS.h"

#include <boost/format.hpp>

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::unit_server;
using namespace chaos::metadata_service::persistence::data_access;

#define US_NEW_INFO INFO_LOG(DeleteUS)
#define US_NEW_DBG  DBG_LOG(DeleteUS)
#define US_NEW_ERR  ERR_LOG(DeleteUS)

CHAOS_MDS_DEFINE_API_CLASS_CD(DeleteUS, "deleteUS")

CDWUniquePtr DeleteUS::execute(CDWUniquePtr api_data) {
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
        if((err = us_da->deleteUS(new_us_uid))){
            LOG_AND_TROW(US_NEW_ERR, -6, boost::str(boost::format("Error removing unit server of id:%1%") % new_us_uid));
        }   
    }
    return CDWUniquePtr();
}
