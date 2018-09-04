/*
 * Copyright 2012, 06/09/2017 INFN
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
#include "UpdatePropertyDefaultValues.h"
#define INFO INFO_LOG(UpdateProperty)
#define DBG  DBG_LOG(UpdateProperty)
#define ERR  ERR_LOG(UpdateProperty)

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::property;
using namespace chaos::metadata_service::api::node;
using namespace chaos::metadata_service::persistence::data_access;

CHAOS_MDS_DEFINE_API_CLASS_CD(UpdatePropertyDefaultValues, "updatePropertyDefaultValues")

CDWUniquePtr UpdatePropertyDefaultValues::execute(CDWUniquePtr api_data) {
    int err = 0;
    ChaosUniquePtr<chaos::common::data::CDataWrapper> node_description;
    CHECK_CDW_THROW_AND_LOG(api_data, ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(api_data, NodeDefinitionKey::NODE_UNIQUE_ID, ERR, -2, "The node unique id is mandatory")
    CHECK_KEY_THROW_AND_LOG(api_data, "property", ERR, -3, "Property update pack is mandatory")
    
    //get the data access
    GET_DATA_ACCESS(NodeDataAccess, n_da, -4)
    
    //get the unique id
    const std::string node_unique_id = api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    
    //
    PropertyGroupVectorSDWrapper pgv_sdv;
    pgv_sdv.serialization_key = "property";
    pgv_sdv.deserialize(api_data.get());
    if((err = n_da->updatePropertyDefaultValue(node_unique_id, pgv_sdv()))) {
        LOG_AND_TROW_FORMATTED(ERR, -4, "Error getting command for uid %1%", %node_unique_id);
    }
    return CDWUniquePtr();
}
