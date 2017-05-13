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

#include <boost/lexical_cast.hpp>

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
    uint64_t command_id = 0;
    //get values array
    std::unique_ptr<CMultiTypeDataArrayWrapper> value_set_array(api_data->getVectorValue("attribute_set_values"));
    
    //get the data access
    GET_DATA_ACCESS(NodeDataAccess, n_da, -3)
    GET_DATA_ACCESS(ControlUnitDataAccess, cu_da, -4)
    for(int idx = 0;
        idx < value_set_array->size();
        idx++){
        std::unique_ptr<CDataWrapper> cu_changes_set(value_set_array->getCDataWrapperElementAtIndex(idx));
        CHECK_KEY_THROW_AND_LOG(cu_changes_set.get(), NodeDefinitionKey::NODE_UNIQUE_ID, CU_SIDAV_ERR, -4, "The ndk_uid key is mandatory")
        CHECK_KEY_THROW_AND_LOG(cu_changes_set.get(), "change_set", CU_SIDAV_ERR, -5, "The change_set key is mandatory")
        //CHECK_KEY_THROW_AND_LOG(set_element.get(), "set_value", CU_SIDAV_ERR, -6, "The cudk_ds_attr_name key is mandatory")
        
        //compose the batch message per control unit
        std::unique_ptr<CDataWrapper> batch_message_per_cu(new CDataWrapper);
        
        //get cu uid
        const std::string cu_uid = cu_changes_set->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
        batch_message_per_cu->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, cu_uid);
        
        //get cu node information for rpc address and instance
        CDataWrapper *cu_node_desc = NULL;
        if((err = n_da->getNodeDescription(cu_uid, &cu_node_desc))){
            LOG_AND_TROW(CU_SIDAV_ERR, err, boost::str(boost::format("Error loading the node description for control unit: %1%") % cu_uid));
        }
        if(cu_node_desc == NULL ){
            //we have no node scription for this control unit so we step forward
            continue;
        }
        
        //we have a descirption for control unit so we need to get the rpc address and domain
        unique_ptr<CDataWrapper> cu_node_description(cu_node_desc);
        if(!cu_node_description->hasKey(NodeDefinitionKey::NODE_RPC_ADDR) ||
           !cu_node_description->hasKey(NodeDefinitionKey::NODE_RPC_DOMAIN)) {
            //we have no node scription for this control unit so we step forward
            continue;
        }
        
        // add cu rpc address
        batch_message_per_cu->addStringValue(NodeDefinitionKey::NODE_RPC_ADDR,
                                             cu_node_description->getStringValue(NodeDefinitionKey::NODE_RPC_ADDR));
        
        //add cu rpc domain
        batch_message_per_cu->addStringValue(NodeDefinitionKey::NODE_RPC_DOMAIN,
                                             cu_node_description->getStringValue(NodeDefinitionKey::NODE_RPC_DOMAIN));
        
        CU_SIDAV_DBG << "Fetch changes_set for control unit:" << cu_uid;
        std::unique_ptr<CMultiTypeDataArrayWrapper> change_set(cu_changes_set->getVectorValue("change_set"));
        
        for(int idx_change = 0;
            idx_change < change_set->size();
            idx_change++) {
            std::unique_ptr<CDataWrapper> change(change_set->getCDataWrapperElementAtIndex(idx_change));
            if(!change->hasKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME)||
               !change->hasKey("change_value")) {
                //we need to have either the values
                continue;
            }
            //attribute description form the dataset
            boost::shared_ptr<CDataWrapper> element_description;
            //get teh attribute name
            const std::string attribute_name = change->getStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME);
            //get the dataset element setup
            if((err = cu_da->getInstanceDatasetAttributeDescription(cu_uid,
                                                                    attribute_name,
                                                                    element_description))) {
                LOG_AND_TROW(CU_SIDAV_ERR, err, boost::str(boost::format("Error loading the description for the the dataset's attribute: %1% for control unit: %2%") % attribute_name % cu_uid));
            }
            
            if(element_description.get()==NULL ||
               !element_description->hasKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE)) {
                //we have no description so we don't have a type for the attribute so we need to step forward
                continue;
            }
            
            //add attribute name with the value in his real type
            try{
                switch(element_description->getInt32Value(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE)){
                    case chaos::DataType::TYPE_BOOLEAN:
                        batch_message_per_cu->addBoolValue(attribute_name,
                                                           boost::lexical_cast<bool>(change->getStringValue("change_value")));
                        break;
                    case chaos::DataType::TYPE_INT32:
                        batch_message_per_cu->addInt32Value(attribute_name,
                                                            boost::lexical_cast<int32_t>(change->getStringValue("change_value")));
                        break;
                    case chaos::DataType::TYPE_INT64:
                        batch_message_per_cu->addInt64Value(attribute_name,
                                                            boost::lexical_cast<int64_t>(change->getStringValue("change_value")));
                        break;
                    case chaos::DataType::TYPE_DOUBLE:
                        batch_message_per_cu->addDoubleValue(attribute_name,
                                                             boost::lexical_cast<double>(change->getStringValue("change_value")));
                        break;
                    case chaos::DataType::TYPE_CLUSTER:{
                       
                        batch_message_per_cu->addJsonValue(attribute_name,change->getStringValue("change_value"));
                    }
                        break;
                    case chaos::DataType::TYPE_STRING:
                        batch_message_per_cu->addStringValue(attribute_name,
                                                             change->getStringValue("change_value"));
                        break;
                    case chaos::DataType::TYPE_BYTEARRAY: {
                        int size = 0;
                        batch_message_per_cu->addBinaryValue(attribute_name,
                                                             change->getBinaryValue("change_value", size),
                                                             size);
                        break;
                    }
                    default:
                        break;
                }
            }catch(...) {
                LOG_AND_TROW(CU_SIDAV_ERR, -5, boost::str(boost::format("Error decoding attribute %1% for control unit %2%") % attribute_name % cu_uid));
            }
        }
        CU_SIDAV_INFO << batch_message_per_cu->getJSONString();
        //submit the changes fo the current control unit
        //launch initilization in background
        command_id = getBatchExecutor()->submitCommand(std::string(GET_MDS_COMMAND_ALIAS(batch::control_unit::ApplyChangeSet)),
                                                       batch_message_per_cu.release());
    }
    return NULL;
}
