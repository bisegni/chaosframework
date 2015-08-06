/*
 *	UpdateProperty.cpp
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
    std::auto_ptr<CDataWrapper> node_description;
    CHECK_CDW_THROW_AND_LOG(api_data, N_UP_ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(api_data, NodeDefinitionKey::NODE_UNIQUE_ID, N_UP_ERR, -2, "The node unique id is mandatory")
    CHECK_KEY_THROW_AND_LOG(api_data, "update_property", N_UP_ERR, -3, "Property update pack is mandatory is mandatory")
    
    //get the data access
    GET_DATA_ACCESS(NodeDataAccess, n_da, -4)
    
    //get the unique id
    const std::string node_unique_id = api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    
    //
    std::auto_ptr<CDataWrapper> update_property_batch_pack(new CDataWrapper());
    update_property_batch_pack->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, node_unique_id);
    api_data->copyKeyTo("update_property", *update_property_batch_pack);
    
    getBatchExecutor()->submitCommand(GET_MDS_COMMAND_ALIAS(batch::node::UpdatePropertyCommand),
                                      update_property_batch_pack.release(),
                                      command_id);
    return NULL;
}
