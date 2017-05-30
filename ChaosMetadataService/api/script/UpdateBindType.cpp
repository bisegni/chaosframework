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

int UpdateBindType::updateBindType(const ScriptBaseDescription& script_base_descrition,
                                   const ScriptInstance& instance) {
    int err = 0;
    ChaosUniquePtr<CDataWrapper>  node_desc_ptr;
    
    GET_DATA_ACCESS(NodeDataAccess, n_da, -1);
    GET_DATA_ACCESS(ScriptDataAccess, s_da, -4);
    
    if(script_base_descrition.unique_id == 0 ||
       script_base_descrition.name.size() == 0) {
        LOG_AND_TROW(ERR, -5, "Invalid script description");
    }
    if(instance.bind_type == ScriptBindTypeUnitServer) {
        CDataWrapper *node_description;
        if((err = n_da->getNodeDescription(instance.bind_node,
                                           &node_description))) {
            ERR << CHAOS_FORMAT("Error searching %1% description",%instance.bind_node);
        }
        node_desc_ptr.reset(node_description);
        if(node_desc_ptr->hasKey(NodeDefinitionKey::NODE_TYPE) == false ||
           node_desc_ptr->isStringValue(NodeDefinitionKey::NODE_TYPE) == false ||
           node_desc_ptr->getStringValue(NodeDefinitionKey::NODE_TYPE).compare(NodeType::NODE_TYPE_UNIT_SERVER) != 0) {
            ERR << CHAOS_FORMAT("Invalid Node Type, the %1% need to be a Unit Server", %instance.bind_node);
        }
    }
    
    //we can proceed
    if((err = s_da->updateBindType(script_base_descrition,
                                   instance))) {
        ERR << CHAOS_FORMAT("Error updating bind type for instance %1% of the script %2% with sequence %3%", %script_base_descrition.name%script_base_descrition.unique_id);
    }
    return err;
}

chaos::common::data::CDataWrapper *UpdateBindType::execute(CDataWrapper *api_data, bool& detach_data) {
    int err = 0;
    CHECK_CDW_THROW_AND_LOG(api_data, ERR, -1, "No parameter found");
    CHECK_KEY_THROW_AND_LOG(api_data, "script_base_description", ERR, -2, "The script_base_description key is mandatory");
    CHECK_ASSERTION_THROW_AND_LOG(api_data->isCDataWrapperValue("script_base_description"), ERR, -3, "script_base_description key need to be an object");
    
    //deserializ base description of the script
    ChaosUniquePtr<CDataWrapper> script_desc_obj(api_data->getCSDataValue("script_base_description"));
    ScriptBaseDescriptionSDWrapper script_description_sdw(script_desc_obj.get());
    if(script_description_sdw().unique_id == 0 ||
       script_description_sdw().name.size() == 0) {
        LOG_AND_TROW(ERR, -4, "Invalid script description");
    }
    if(api_data->hasKey("vector_script_instance")) {
        CHECK_ASSERTION_THROW_AND_LOG(api_data->isVectorValue("vector_script_instance"), ERR, -5, "vector_script_instance key need to be a vector of object");
        StdVectorSDWrapper<ScriptInstance, ScriptInstanceSDWrapper> ni_list_wrapper;
        ni_list_wrapper.serialization_key = "vector_script_instance";
        ni_list_wrapper.deserialize(api_data);
        for(StdVectorSDWrapper<ScriptInstance, ScriptInstanceSDWrapper>::iterator it = ni_list_wrapper().begin(),
            end = ni_list_wrapper().end();
            it != end;
            it++) {
            if((err = updateBindType(script_description_sdw(),
                                     *it))) {
                LOG_AND_TROW(ERR, -6, CHAOS_FORMAT("Error updating bind for %1% instance of script %2%[%3%]", %it->instance_name%script_description_sdw().name%script_description_sdw().unique_id));
            }
        }
    } else if (api_data->hasKey("script_instance")) {
        CHECK_ASSERTION_THROW_AND_LOG(api_data->isCDataWrapperValue("script_base_description"), ERR, -6, "script_base_description key need to be an object");
        ChaosUniquePtr<CDataWrapper> instance_ptr(api_data->getCSDataValue("script_base_description"));
        ScriptInstanceSDWrapper isdw(instance_ptr.get());
        if((err = updateBindType(script_description_sdw(),
                                 isdw()))) {
            LOG_AND_TROW(ERR, -7, CHAOS_FORMAT("Error updating bind for %1% instance of script %2%[%3%]", %isdw().instance_name%script_description_sdw().name%script_description_sdw().unique_id));
        }
    }
    return NULL;
}
