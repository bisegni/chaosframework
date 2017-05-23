/*
 *	UpdateBindType.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 23/05/2017 INFN, National Institute of Nuclear Physics
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

#include "UpdateBindType.h"

#include <chaos/common/utility/TimingUtil.h>

#include <chaos_service_common/data/data.h>

#define INFO INFO_LOG(UpdateBindType)
#define DBG  DBG_LOG(UpdateBindType)
#define ERR  ERR_LOG(UpdateBindType)

using namespace chaos::common::data;
using namespace chaos::common::network;
using namespace chaos::common::utility;
using namespace chaos::common::event::channel;
using namespace chaos::common::data::structured;

using namespace chaos::service_common::data::script;

using namespace chaos::metadata_service::api::script;
using namespace chaos::metadata_service::persistence::data_access;

UpdateBindType::UpdateBindType():
AbstractApi("updateBindType"){}

UpdateBindType::~UpdateBindType() {}

chaos::common::data::CDataWrapper *UpdateBindType::execute(CDataWrapper *api_data, bool& detach_data) {
    CHECK_CDW_THROW_AND_LOG(api_data, ERR, -1, "No parameter found");
    CHECK_KEY_THROW_AND_LOG(api_data, "instance_name", ERR, -2, "The instance name is mandatory");
    CHECK_ASSERTION_THROW_AND_LOG(api_data->isStringValue("instance_name"), ERR, -3, "The instance_name key need to be a string")
    
    ChaosUniquePtr<chaos::common::data::CDataWrapper> batch_data(api_data);
    detach_data = true;
    
    GET_DATA_ACCESS(NodeDataAccess, n_da, -3);
    GET_DATA_ACCESS(ScriptDataAccess, s_da, -4);
    
    int err = 0;
    ChaosUniquePtr<CDataWrapper> node_desc_ptr;
    const std::string instance_name = api_data->getStringValue("instance_name");
    const ScriptBindType scb = (batch_data->hasKey("script_bind_type")?static_cast<ScriptBindType>(batch_data->isInt32Value("script_bind_type")):ScriptBindTypeDisable);
    const std::string node_parent = CDW_GET_SRT_WITH_DEFAULT(batch_data.get(),NodeDefinitionKey::NODE_PARENT, "");
    //deserializ base description of the script
    ScriptBaseDescriptionSDWrapper script_description_sdw(batch_data.get());
    if(script_description_sdw().unique_id == 0 ||
       script_description_sdw().name.size() == 0) {
        LOG_AND_TROW(ERR, -5, "Invalid script description");
    }
    if(scb == ScriptBindTypeUnitServer) {
        CDataWrapper *node_description;
        if((err = n_da->getNodeDescription(node_parent,
                                           &node_description))) {
            LOG_AND_TROW(ERR, err , CHAOS_FORMAT("Error searching %1% description",%node_parent));
        }
        node_desc_ptr.reset(node_description);
        if(node_desc_ptr->hasKey(NodeDefinitionKey::NODE_TYPE) == false ||
           node_desc_ptr->isStringValue(NodeDefinitionKey::NODE_TYPE) == false ||
           node_desc_ptr->getStringValue(NodeDefinitionKey::NODE_TYPE).compare(NodeType::NODE_TYPE_UNIT_SERVER) != 0) {
            LOG_AND_TROW(ERR, -6, CHAOS_FORMAT("Invalid Node Type, the %1% need to be a Unit Server", %node_parent));
        }
    }
    
    //we can proceed
    if((err = s_da->updateBindType(script_description_sdw().unique_id,
                                   script_description_sdw().name,
                                   instance_name,
                                   scb,
                                   node_parent))) {
        LOG_AND_TROW(ERR, -6, CHAOS_FORMAT("Error updating bind type for instance %1% of the script %2% with sequence %3%", %script_description_sdw().name%script_description_sdw().unique_id));
    }
    
    
    return NULL;
}
