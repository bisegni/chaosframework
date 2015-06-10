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
using namespace chaos::common::batch_command;
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
    CHECK_CDW_THROW_AND_LOG(api_data, CU_SCT_ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(api_data, "template_list", CU_SCT_ERR, -2, "The attribute template_list is mandatory")
    
    GET_DATA_ACCESS(NodeDataAccess, n_da, -3)
    GET_DATA_ACCESS(ControlUnitDataAccess, cu_da, -4)
    
    std::auto_ptr<CMultiTypeDataArrayWrapper> tempalte_list(api_data->getVectorValue("template_list"));
    
    for(int idx = 0;
        idx < tempalte_list->size();
        idx++) {
        
        std::auto_ptr<CDataWrapper> template_element(tempalte_list->getCDataWrapperElementAtIndex(idx));
        
        if(template_element->hasKey("template_name")&&
           template_element->hasKey(BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID)) {
            CU_SCT_ERR << "Tempalte with no all mandatory key: " << template_element->getJSONData();
            continue;
        }
        
        const std::string template_name = template_element->getStringValue("template_name");
        const std::string command_unique_id = template_element->getStringValue(BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID);
        if((err = n_da->setCommandTemplate(*template_element))) {
            LOG_AND_TROW_FORMATTED(CU_SCT_ERR, -5, "Error setting the template %1% of command uid %2%", %template_name%command_unique_id)
        }
    }
    return NULL;
}