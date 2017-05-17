/*
 *	GetFullDescription.cpp
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

#include "GetFullDescription.h"

#include <boost/format.hpp>
#include <boost/foreach.hpp>

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::control_unit;
using namespace chaos::metadata_service::persistence::data_access;

#define CU_GCD_INFO INFO_LOG(GetFullDescription)
#define CU_GCD_DBG  DBG_LOG(GetFullDescription)
#define CU_GCD_ERR  ERR_LOG(GetFullDescription)

GetFullDescription::GetFullDescription():
AbstractApi("getFullDescription"){
    
}

GetFullDescription::~GetFullDescription() {
    
}

CDataWrapper *GetFullDescription::execute(CDataWrapper *api_data,
                                         bool& detach_data) throw(chaos::CException) {
    
    if(!api_data) {LOG_AND_TROW(CU_GCD_ERR, -1, "Search parameter are needed");}
    if(!api_data->hasKey(chaos::NodeDefinitionKey::NODE_UNIQUE_ID)) {LOG_AND_TROW(CU_GCD_ERR, -2, "The ndk_unique_id key is mandatory");}
    
    int                 err         = 0;
    bool                presence    = false;
    CDataWrapper        *result     = NULL;
    const   std::string cu_uid      = api_data->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID);
    bool 	return_all=false;
    if( api_data->hasKey("all")){
    	return_all=api_data->getBoolValue("all");
    }
    GET_DATA_ACCESS(ControlUnitDataAccess, cu_da, -3)
    GET_DATA_ACCESS(DataServiceDataAccess, ds_da, -4)
    if(return_all){
        if((err = cu_da->getFullDescription(cu_uid, &result))||(result==NULL)) {
               LOG_AND_TROW(CU_GCD_ERR, err, boost::str(boost::format("Error fetching the dataset for the node  unit uid:%1% with error %2%") % cu_uid % err));
        }
        ChaosUniquePtr<CDataWrapper> dataset(result);
        return dataset.release();

     }

    
    //get default control unit node description
    if((err = cu_da->checkPresence(cu_uid, presence))) {
        LOG_AND_TROW(CU_GCD_ERR, err, boost::str(boost::format("Error fetching the presence of the control unit id:%1% with error %2%") % cu_uid % err));
    } else if(!presence) {
        LOG_AND_TROW(CU_GCD_ERR, err, boost::str(boost::format("No control unit available with uid id:%1%") % cu_uid));
    }
    
    if((err = cu_da->checkDatasetPresence(cu_uid, presence))) {
        LOG_AND_TROW(CU_GCD_ERR, err, boost::str(boost::format("Error fetching the presence of the dataset for the control unit uid:%1% with error %2%") % cu_uid % err));
    } else if(!presence) {
        LOG_AND_TROW(CU_GCD_ERR, -10000, boost::str(boost::format("No dataset found for control unit with uid id:%1%") % cu_uid));
    }


    if((err = cu_da->getDataset(cu_uid, &result))) {
        LOG_AND_TROW(CU_GCD_ERR, err, boost::str(boost::format("Error fetching the dataset for the control unit uid:%1% with error %2%") % cu_uid % err));
    }
    if(result == NULL) {
        LOG_AND_TROW(CU_GCD_ERR, -10001, boost::str(boost::format("Dataset not found for control unit '%1%'") % cu_uid));
    }
    //we have data set and now we need to update the input attribute
    ChaosUniquePtr<CDataWrapper> dataset(result);

    ChaosUniquePtr<CDataWrapper> init_datapack(new CDataWrapper());
    ChaosUniquePtr<CDataWrapper> init_dataset(new CDataWrapper());
    
    init_datapack->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, cu_uid);
    
    if(dataset->hasKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION) && dataset->isVector(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION)) {
        ChaosUniquePtr<CMultiTypeDataArrayWrapper> dataset_element_vec(dataset->getVectorValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION));
        for(int idx = 0; idx <
            dataset_element_vec->size();
            idx++) {
            //get the dataset element
            ChaosSharedPtr<CDataWrapper> element(dataset_element_vec->getCDataWrapperElementAtIndex(idx));
            const std::string  ds_attribute_name = element->getStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME);
            int32_t direction = element->getInt32Value(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DIRECTION);
            
            ChaosSharedPtr<CDataWrapper> element_configuration;
            //get the dataset element setup
            if((err = cu_da->getInstanceDatasetAttributeConfiguration(cu_uid,
                                                                      ds_attribute_name,
                                                                      element_configuration))) {
                LOG_AND_TROW(CU_GCD_ERR, err, boost::str(boost::format("Error loading the configuration for the the dataset's attribute: %1% for control unit: %2%") % ds_attribute_name % cu_uid));
            } else if((direction == chaos::DataType::Input ||
                       direction == chaos::DataType::Bidirectional) &&
                      element_configuration.get() != NULL){
                //we can retrive the configured attribute
                ChaosSharedPtr<CDataWrapper> init_ds_attribute = mergeDatasetAttributeWithSetup(element,
                                                                                                   element_configuration);
                init_dataset->appendCDataWrapperToArray(*init_ds_attribute.get());
            } else {
                init_dataset->appendCDataWrapperToArray(*element.get());
            }
        }
    }
    init_dataset->finalizeArrayForKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION);
    
    init_datapack->addCSDataValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION, *init_dataset);

    		//now search for data service associated
    std::vector<std::string> associated_ds;
    //load the asosciated dataservice
    if((err = cu_da->getDataServiceAssociated(cu_uid,
                                              associated_ds))){
        LOG_AND_TROW(CU_GCD_ERR, err, boost::str(boost::format("Error loading the associated data service for the control unit %1%") % cu_uid));
    }
    
    //check if we have found some
    if(associated_ds.size() == 0) {
        CU_GCD_DBG << "No dataservice has been found for control unit:" << cu_uid <<" so we need to get the best tree of them";
        //no we need to get tbest tree available cds to retun publishable address
        if((err = ds_da->getBestNDataService(associated_ds, 3))) {
            LOG_AND_TROW(CU_GCD_ERR, err, boost::str(boost::format("Error fetching %2% best available data service for inititializing the control unit:%1%") % cu_uid % 3));
        }
    }
    
    //add endpoint(data service) where the control unit need to publish his dataset
    if(associated_ds.size()) {
        //add data service where published data
        BOOST_FOREACH(std::string ds_unique_id, associated_ds) {
            CDataWrapper *ds_description = NULL;
            if((err = ds_da->getDescription(ds_unique_id, &ds_description))) {
                LOG_AND_TROW(CU_GCD_ERR, err, boost::str(boost::format("Error fetchin description for data service %1%") % ds_unique_id));
            } else if(ds_description == NULL) {
                CU_GCD_DBG << "No description foudn for data service:" << ds_unique_id;
            } else {
                ChaosUniquePtr<CDataWrapper> ds_object(ds_description);
                if(ds_object->hasKey(NodeDefinitionKey::NODE_DIRECT_IO_ADDR) &&
                   ds_object->hasKey(DataServiceNodeDefinitionKey::DS_DIRECT_IO_ENDPOINT)) {
                    //we can create the address
                    std::string direct_io_addr = boost::str(boost::format("%1%|%2%") %
                                                            ds_object->getStringValue(NodeDefinitionKey::NODE_DIRECT_IO_ADDR) %
                                                            ds_object->getInt32Value(DataServiceNodeDefinitionKey::DS_DIRECT_IO_ENDPOINT));
                    init_datapack->appendStringToArray(direct_io_addr);
                }
                
            }
            
        }
        init_datapack->finalizeArrayForKey(DataServiceNodeDefinitionKey::DS_DIRECT_IO_FULL_ADDRESS_LIST);
    }
    return init_datapack.release();
}

ChaosSharedPtr<CDataWrapper> GetFullDescription::mergeDatasetAttributeWithSetup(ChaosSharedPtr<CDataWrapper> element_in_dataset,
                                                                                  ChaosSharedPtr<CDataWrapper> element_in_setup) {
    ChaosSharedPtr<CDataWrapper> result(new CDataWrapper());
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
