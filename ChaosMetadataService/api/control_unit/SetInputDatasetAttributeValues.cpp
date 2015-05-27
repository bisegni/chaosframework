/*
 *	SetInputDatasetAttributeValues.cpp
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

#include "SetInputDatasetAttributeValues.h"

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::control_unit;
using namespace chaos::metadata_service::persistence::data_access;

#define CU_SIDAV_INFO INFO_LOG(SetInputDatasetAttributeValues)
#define CU_SIDAV_DBG  DBG_LOG(SetInputDatasetAttributeValues)
#define CU_SIDAV_ERR  ERR_LOG(SetInputDatasetAttributeValues)


SetInputDatasetAttributeValues::SetInputDatasetAttributeValues():
AbstractApi("setInputDatasetAttributeValues"){
    
}

SetInputDatasetAttributeValues::~SetInputDatasetAttributeValues() {
    
}

CDataWrapper *SetInputDatasetAttributeValues::execute(CDataWrapper *api_data,
                                                      bool& detach_data) throw(chaos::CException) {
    CHECK_CDW_THROW_AND_LOG(api_data, CU_SIDAV_ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(api_data, "attribute_set_values", CU_SIDAV_ERR, -2, "The attribute_set_values key is mandatory")
    
    int err = 0;
    //get values array
    std::auto_ptr<CMultiTypeDataArrayWrapper> value_set_array(api_data->getVectorValue("attribute_set_values"));
    
    //get the data access
    GET_DATA_ACCESS(ControlUnitDataAccess, cu_da, -3)
    
    for(int idx = 0;
        idx < value_set_array->size();
        idx++){
        std::auto_ptr<CDataWrapper> set_element(value_set_array->getCDataWrapperElementAtIndex(idx));
        CHECK_KEY_THROW_AND_LOG(set_element.get(), NodeDefinitionKey::NODE_UNIQUE_ID, CU_SIDAV_ERR, -4, "The ndk_uid key is mandatory")
        CHECK_KEY_THROW_AND_LOG(set_element.get(), ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME, CU_SIDAV_ERR, -5, "The cudk_ds_attr_name key is mandatory")
        CHECK_KEY_THROW_AND_LOG(set_element.get(), "set_value", CU_SIDAV_ERR, -6, "The cudk_ds_attr_name key is mandatory")
        
        const std::string cu_uid = set_element->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
        const std::string attribute_name = set_element->getStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME);
        //attribute description form the dataset
        boost::shared_ptr<CDataWrapper> element_description;
        //get the dataset element setup
        if((err = cu_da->getInstanceDatasetAttributeDescription(cu_uid,
                                                                attribute_name,
                                                                element_description))) {
            LOG_AND_TROW(CU_SIDAV_ERR, err, boost::str(boost::format("Error loading the description for the the dataset's attribute: %1% for control unit: %2%") % attribute_name % cu_uid));
        }
        
    }
    return NULL;
}
