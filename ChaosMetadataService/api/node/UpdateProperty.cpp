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

#include "UpdateProperty.h"
#include "../../batch/node/UpdatePropertyCommand.h"

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::batch_command;
using namespace chaos::metadata_service::api::node;
using namespace chaos::metadata_service::persistence::data_access;

#define N_UP_INFO INFO_LOG(UpdateProperty)
#define N_UP_DBG  DBG_LOG(UpdateProperty)
#define N_UP_ERR  ERR_LOG(UpdateProperty)

UpdateProperty::UpdateProperty():
AbstractApi("updateProperty"){}

UpdateProperty::~UpdateProperty(){}

CDataWrapper *UpdateProperty::execute(CDataWrapper *api_data,
                                      bool& detach_data) throw(chaos::CException) {
    uint64_t command_id;
    ChaosUniquePtr<chaos::common::data::CDataWrapper> node_description;
    CHECK_CDW_THROW_AND_LOG(api_data, N_UP_ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(api_data, NodeDefinitionKey::NODE_UNIQUE_ID, N_UP_ERR, -2, "The node unique id is mandatory")
    CHECK_KEY_THROW_AND_LOG(api_data, "update_property", N_UP_ERR, -3, "Property update pack is mandatory")
    
    //get the data access
    GET_DATA_ACCESS(NodeDataAccess, n_da, -4)
    
    //get the unique id
    const std::string node_unique_id = api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    
    //
    ChaosUniquePtr<chaos::common::data::CDataWrapper> update_property_batch_pack(new CDataWrapper());
    update_property_batch_pack->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, node_unique_id);
    api_data->copyKeyTo("update_property", *update_property_batch_pack);
    
    command_id = getBatchExecutor()->submitCommand(GET_MDS_COMMAND_ALIAS(batch::node::UpdatePropertyCommand),
                                                   update_property_batch_pack.release());
    return NULL;
}
