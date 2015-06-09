/*
 *	SetCommandTemplate.cpp
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

#include "SetCommandTemplate.h"


#include <boost/format.hpp>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::metadata_service::api::control_unit;
using namespace chaos::metadata_service::persistence::data_access;

#define CU_SCT_INFO INFO_LOG(SetCommandTemplate)
#define CU_SCT_DBG  DBG_LOG(SetCommandTemplate)
#define CU_SCT_ERR  ERR_LOG(SetCommandTemplate)

SetCommandTemplate::SetCommandTemplate():
AbstractApi("setCommandTemplate"){
    
}

SetCommandTemplate::~SetCommandTemplate() {
    
}

CDataWrapper *SetCommandTemplate::execute(CDataWrapper *api_data,
                                          bool& detach_data) throw(chaos::CException) {
    int err = 0;
    bool presence = false;
    CHECK_CDW_THROW_AND_LOG(api_data, CU_SCT_ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(api_data, NodeDefinitionKey::NODE_UNIQUE_ID, CU_SCT_ERR, -2, boost::str(boost::format("The attribute %1% is mandatory")%NodeDefinitionKey::NODE_UNIQUE_ID))
    CHECK_KEY_THROW_AND_LOG(api_data, "template_name", CU_SCT_ERR, -3, "The attribute template_name is mandatory")
    CHECK_KEY_THROW_AND_LOG(api_data, chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_ALIAS, CU_SCT_ERR, -4, boost::str(boost::format("The attribute %1% is mandatory")%chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_ALIAS))
    
    GET_DATA_ACCESS(NodeDataAccess, n_da, -5)
    GET_DATA_ACCESS(ControlUnitDataAccess, cu_da, -6)
    
    const std::string cu_uid = api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    const std::string template_name = api_data->getStringValue("template_name");
    const std::string command_alias = api_data->getStringValue(chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_ALIAS);
    if((err = n_da->checkNodePresence(presence,
                                      cu_uid))) {
        
    }
    
    return NULL;
}