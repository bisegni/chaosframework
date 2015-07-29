/*
 *	GetCurrentDataset.cpp
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

#include "GetCurrentDataset.h"

#include <boost/format.hpp>

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::control_unit;
using namespace chaos::metadata_service::persistence::data_access;

#define CU_GCD_INFO INFO_LOG(StartStop)
#define CU_GCD_DBG  DBG_LOG(StartStop)
#define CU_GCD_ERR  ERR_LOG(StartStop)

GetCurrentDataset::GetCurrentDataset():
AbstractApi("getCurrentDataset"){
    
}

GetCurrentDataset::~GetCurrentDataset() {
    
}

CDataWrapper *GetCurrentDataset::execute(CDataWrapper *api_data,
                                         bool& detach_data) throw(chaos::CException) {
    
    if(!api_data) {LOG_AND_TROW(CU_GCD_ERR, -1, "Search parameter are needed");}
    if(!api_data->hasKey(chaos::NodeDefinitionKey::NODE_UNIQUE_ID)) {LOG_AND_TROW(CU_GCD_ERR, -2, "The ndk_unique_id key is mandatory");}
    
    int                 err         = 0;
    bool                presence    = false;
    CDataWrapper        *result     = NULL;
    const   std::string cu_uid      = api_data->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID);
    
    GET_DATA_ACCESS(ControlUnitDataAccess, cu_da, -3)
    
    //get default control unit node description
    if((err = cu_da->checkPresence(cu_uid, presence))) {
        LOG_AND_TROW(CU_GCD_ERR, err, boost::str(boost::format("Error fetching the presence of the control unit id:%1% with error %2%") % cu_uid % err));
    } else if(!presence) {
        LOG_AND_TROW(CU_GCD_ERR, err, boost::str(boost::format("No control unit available with uid id:%1%") % cu_uid));
    }
    
    if((err = cu_da->checkDatasetPresence(cu_uid, presence))) {
        LOG_AND_TROW(CU_GCD_ERR, err, boost::str(boost::format("Error fetching the presence of the dataset for the control unit uid:%1% with error %2%") % cu_uid % err));
    } else if(!presence) {
        LOG_AND_TROW(CU_GCD_ERR, err, boost::str(boost::format("No dataset found for control unit with uid id:%1%") % cu_uid));
    }
    if((err = cu_da->getDataset(cu_uid, &result))) {
        LOG_AND_TROW(CU_GCD_ERR, err, boost::str(boost::format("Error fetching the dataset for the control unit uid:%1% with error %2%") % cu_uid % err));
    }
    if(result == NULL) {
        LOG_AND_TROW(CU_GCD_ERR, err, boost::str(boost::format("Dataset not found for control unit '%1%'") % cu_uid));
    }
    //we have data set and now we need to update the input attribute
    std::auto_ptr<CDataWrapper> dataset(result);
    std::auto_ptr<CDataWrapper> init_dataset(new CDataWrapper());

    if(dataset->hasKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION) && dataset->isVector(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION)) {
    std:auto_ptr<CMultiTypeDataArrayWrapper> dataset_element_vec(dataset->getVectorValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION));
        for(int idx = 0; idx <
            dataset_element_vec->size();
            idx++) {
            //get the dataset element
            boost::shared_ptr<CDataWrapper> element(dataset_element_vec->getCDataWrapperElementAtIndex(idx));
            const std::string  ds_attribute_name = element->getStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME);
            int32_t direction = element->getInt32Value(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DIRECTION);
            
            boost::shared_ptr<CDataWrapper> element_configuration;
            //get the dataset element setup
            if((err = cu_da->getInstanceDatasetAttributeConfiguration(cu_uid,
                                                                      ds_attribute_name,
                                                                      element_configuration))) {
                LOG_AND_TROW(CU_GCD_ERR, err, boost::str(boost::format("Error loading the configuration for the the dataset's attribute: %1% for control unit: %2%") % ds_attribute_name % cu_uid));
            } else if((direction == chaos::DataType::Input ||
                       direction == chaos::DataType::Bidirectional) &&
                      element_configuration.get() != NULL){
                //we can retrive the configured attribute
                boost::shared_ptr<CDataWrapper> init_ds_attribute = mergeDatasetAttributeWithSetup(element,
                                                                                                   element_configuration);
                init_dataset->appendCDataWrapperToArray(*init_ds_attribute.get());
            } else {
                init_dataset->appendCDataWrapperToArray(*element.get());
            }
        }
    }
    init_dataset->finalizeArrayForKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION);
    return init_dataset.release();
}

boost::shared_ptr<CDataWrapper> GetCurrentDataset::mergeDatasetAttributeWithSetup(boost::shared_ptr<CDataWrapper> element_in_dataset,
                                                                                  boost::shared_ptr<CDataWrapper> element_in_setup) {
    boost::shared_ptr<CDataWrapper> result(new CDataWrapper());
    //move
    MOVE_STRING_VALUE(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME, element_in_dataset, result)
    MOVE_STRING_VALUE(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DESCRIPTION, element_in_dataset, result)
    MOVE_INT32_VALUE(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE, element_in_dataset, result)
    MOVE_INT32_VALUE(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DIRECTION, element_in_dataset, result)
    MOVE_INT32_VALUE(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_VALUE_MAX_SIZE, element_in_dataset, result)
    
    //update config value
    MERGE_STRING_VALUE(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DEFAULT_VALUE, element_in_dataset, element_in_setup, result)
    MERGE_STRING_VALUE(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MAX_RANGE, element_in_dataset, element_in_setup, result)
    MERGE_STRING_VALUE(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MIN_RANGE, element_in_dataset, element_in_setup, result)
    return result;
}