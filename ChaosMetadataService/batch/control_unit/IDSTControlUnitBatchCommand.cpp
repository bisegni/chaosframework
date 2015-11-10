/*
 *	IDSTControlUnitBatchCommand.cpp
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

#include "IDSTControlUnitBatchCommand.h"

#include <boost/foreach.hpp>

using namespace chaos::common::data;
using namespace chaos::common::network;
using namespace chaos::metadata_service::batch::control_unit;

#define CU_IDST_BC_INFO INFO_LOG(IDSTControlUnitBatchCommand)
#define CU_IDST_BC_DBG  DBG_LOG(IDSTControlUnitBatchCommand)
#define CU_IDST_BC_ERR  ERR_LOG(IDSTControlUnitBatchCommand)

#define MOVE_STRING_VALUE(k, src, dst)\
if(src->hasKey(k)) {\
dst->addStringValue(k, src->getStringValue(k));\
}

#define MERGE_STRING_VALUE(k, src, src2, dst)\
if(src2->hasKey(k)) {\
dst->addStringValue(k, src2->getStringValue(k));\
} else {\
MOVE_STRING_VALUE(k, src, dst)\
}

#define MOVE_INT32_VALUE(k, src, dst)\
if(src->hasKey(k)) {\
dst->addInt32Value(k, src->getInt32Value(k));\
}


DEFINE_MDS_COMAMND_ALIAS(IDSTControlUnitBatchCommand)


IDSTControlUnitBatchCommand::IDSTControlUnitBatchCommand():
MDSBatchCommand(),
retry_number(0),
cu_id(),
action(ACTION_INIT) {}

IDSTControlUnitBatchCommand::~IDSTControlUnitBatchCommand() {}

// inherited method
void IDSTControlUnitBatchCommand::setHandler(CDataWrapper *data) {
    MDSBatchCommand::setHandler(data);
    
    int err = 0;
    CDataWrapper *tmp_ptr = NULL;
    
    //set cu id to the batch command datapack
    cu_id = data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    action = static_cast<IDSTAction>(data->getInt32Value("action"));//0-init,1-start,2-ztop,3-deinit
    
    if((err = getDataAccess<mds_data_access::NodeDataAccess>()->getNodeDescription(cu_id, &tmp_ptr))) {
        LOG_AND_TROW(CU_IDST_BC_ERR, -1, "Control Unit information has not been found!")
    } else if(tmp_ptr != NULL) {
        std::auto_ptr<CDataWrapper> auto_node_info(tmp_ptr);
        if(auto_node_info->hasKey(NodeDefinitionKey::NODE_RPC_ADDR) &&
           auto_node_info->hasKey(NodeDefinitionKey::NODE_RPC_DOMAIN) ){
            request = createRequest(auto_node_info->getStringValue(NodeDefinitionKey::NODE_RPC_ADDR),
                                    auto_node_info->getStringValue(NodeDefinitionKey::NODE_RPC_DOMAIN),
                                    "");
        }else{
            LOG_AND_TROW(CU_IDST_BC_ERR, -2, "Control Unit information doesn't has the RPC address!")
        }
        
    } else {
        LOG_AND_TROW(CU_IDST_BC_ERR, -3, "Control Unit information not found!")
    }
}

// inherited method
void IDSTControlUnitBatchCommand::acquireHandler() {
    MDSBatchCommand::acquireHandler();
}

// inherited method
void IDSTControlUnitBatchCommand::ccHandler() {
    MDSBatchCommand::ccHandler();
    switch(request->phase) {
        case MESSAGE_PHASE_UNSENT: {
            switch(action) {
                case ACTION_INIT:
                    CU_IDST_BC_INFO << "Send init command to " << cu_id;
                    request->remote_action = NodeDomainAndActionRPC::ACTION_NODE_INIT;
                    message = initialize(cu_id);
                    break;
                case ACTION_START:
                    CU_IDST_BC_INFO << "Send start command to " << cu_id;
                    request->remote_action = NodeDomainAndActionRPC::ACTION_NODE_START;
                    message = start(cu_id);
                    break;
                case ACTION_STOP:
                    CU_IDST_BC_INFO << "Send stop command to " << cu_id;
                    request->remote_action = NodeDomainAndActionRPC::ACTION_NODE_STOP;
                    message = stop(cu_id);
                    break;
                case ACTION_DEINIT:
                    CU_IDST_BC_INFO << "Send deinit command to " << cu_id;
                    request->remote_action = NodeDomainAndActionRPC::ACTION_NODE_DEINIT;
                    message = deinitialize(cu_id);
                    break;
            }
            
            //send message for action
            sendRequest(*request,
                        message.get());
            break;
        }
        case MESSAGE_PHASE_SENT: {
            manageRequestPhase(*request);
            break;
        }
            
        case MESSAGE_PHASE_COMPLETED:
        case MESSAGE_PHASE_TIMEOUT: {
            BC_END_RUNNIG_PROPERTY
            break;
        }
    }
}
// inherited method
bool IDSTControlUnitBatchCommand::timeoutHandler() {
    bool result = MDSBatchCommand::timeoutHandler();
    return result;
}

boost::shared_ptr<CDataWrapper> IDSTControlUnitBatchCommand::mergeDatasetAttributeWithSetup(boost::shared_ptr<CDataWrapper> element_in_dataset,
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
    DEBUG_CODE(CU_IDST_BC_INFO << element_in_dataset->getJSONString();)
    DEBUG_CODE(CU_IDST_BC_INFO << element_in_setup->getJSONString();)
    DEBUG_CODE(CU_IDST_BC_INFO << result->getJSONString();)
    return result;
}

std::auto_ptr<CDataWrapper> IDSTControlUnitBatchCommand::initialize(const std::string& cu_uid) {
    int err = 0;
    CDataWrapper *result = NULL;
    std::auto_ptr<CDataWrapper> cu_base_description;
    std::auto_ptr<CDataWrapper> dataset_description;
    std::auto_ptr<CDataWrapper> init_datapack(new CDataWrapper());
    
    //set the control unique id in the init datapack
    init_datapack->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, cu_uid);
    
    //get default control unit node description
    if((err = getDataAccess<mds_data_access::NodeDataAccess>()->getNodeDescription(cu_uid, &result))) {
        LOG_AND_TROW(CU_IDST_BC_ERR, err, boost::str(boost::format("Error fetching the control unit node default description for unique id:%1% with error %2%") % cu_uid));
    }
    cu_base_description.reset(result); result=NULL;
    
    //copy rpc information in the init datapack
    CHECK_KEY_THROW_AND_LOG(cu_base_description, NodeDefinitionKey::NODE_RPC_ADDR, CU_IDST_BC_ERR, -3, "No rpc addres in the control unit descirption")
    init_datapack->addStringValue(NodeDefinitionKey::NODE_RPC_ADDR, cu_base_description->getStringValue(NodeDefinitionKey::NODE_RPC_ADDR));
    
    CHECK_KEY_THROW_AND_LOG(cu_base_description, NodeDefinitionKey::NODE_RPC_DOMAIN, CU_IDST_BC_ERR, -3, "No rpc domain in the control unit descirption")
    init_datapack->addStringValue(NodeDefinitionKey::NODE_RPC_DOMAIN, cu_base_description->getStringValue(NodeDefinitionKey::NODE_RPC_DOMAIN));
    
    if((err = getDataAccess<mds_data_access::ControlUnitDataAccess>()->getInstanceDescription(cu_uid,
                                                                                              &result))){
        LOG_AND_TROW(CU_IDST_BC_ERR, err, boost::str(boost::format("Error fetching the control unit instance description for control unit:%1%") % cu_uid));
    } else if(result == NULL) {
        LOG_AND_TROW(CU_IDST_BC_ERR, err, boost::str(boost::format("No instance found for control unit: %1%") % cu_uid));
    }
    //we have the configured instance
    std::auto_ptr<CDataWrapper> instance_description(result);
    result = NULL;
    
    //get the dataset of the control unit
    if((err = getDataAccess<mds_data_access::ControlUnitDataAccess>()->getDataset(cu_uid,
                                                                                  &result))) {
        LOG_AND_TROW(CU_IDST_BC_ERR, err, boost::str(boost::format("Error fetching dataset for control unit %1%") % cu_uid));
    } else if(result == NULL){
        LOG_AND_TROW(CU_IDST_BC_ERR, err, boost::str(boost::format("No dataset found for control unit: %1%") % cu_uid));
    }
    
    //we have the published dataset
    dataset_description.reset(result);
    
    std::vector<std::string> associated_ds;
    //load the asosciated dataservice
    if((err = getDataAccess<mds_data_access::ControlUnitDataAccess>()->getDataServiceAssociated(cu_uid,
                                                                                                associated_ds))){
        LOG_AND_TROW(CU_IDST_BC_ERR, err, boost::str(boost::format("Error loading the associated data service for the control unit %1%") % cu_uid));
    }
    
    //check if we have found some
    if(associated_ds.size() == 0) {
        CU_IDST_BC_DBG << "No dataservice has been found for control unit:" << cu_uid <<" so we need to get the best tree of them";
        //no we need to get tbest tree available cds to retun publishable address
        if((err = getDataAccess<mds_data_access::DataServiceDataAccess>()->getBestNDataService(associated_ds, 3))) {
            LOG_AND_TROW(CU_IDST_BC_ERR, err, boost::str(boost::format("Error fetching %2% best available data service for inititializing the control unit:%1%") % cu_uid % 3));
        }
    }
    
    //whe have all now we can compose the datapack to send in ckaground
std:auto_ptr<CMultiTypeDataArrayWrapper> dataset_element_vec(dataset_description->getVectorValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION));
    std::auto_ptr<CDataWrapper> init_dataset(new CDataWrapper());
    for(int idx = 0; idx <
        dataset_element_vec->size();
        idx++) {
        //get the dataset element
        boost::shared_ptr<CDataWrapper> element(dataset_element_vec->getCDataWrapperElementAtIndex(idx));
        const std::string  ds_attribute_name = element->getStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME);
        int32_t direction = element->getInt32Value(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DIRECTION);
        
        boost::shared_ptr<CDataWrapper> element_configuration;
        //get the dataset element setup
        if((err = getDataAccess<mds_data_access::ControlUnitDataAccess>()->getInstanceDatasetAttributeConfiguration(cu_uid,
                                                                                                                    ds_attribute_name,
                                                                                                                    element_configuration))) {
            LOG_AND_TROW(CU_IDST_BC_ERR, err, boost::str(boost::format("Error loading the configuration for the the dataset's attribute: %1% for control unit: %2%") % ds_attribute_name % cu_uid));
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
    init_dataset->finalizeArrayForKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION);
    
    //add configurad dataset to the init datapack
    init_datapack->addCSDataValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION, *init_dataset.get());
    
    //add endpoint(data service) where the control unit need to publish his dataset
    if(associated_ds.size()) {
        //add data service where published data
        BOOST_FOREACH(std::string ds_unique_id, associated_ds) {
            CDataWrapper *ds_description = NULL;
            if((err = getDataAccess<mds_data_access::DataServiceDataAccess>()->getDescription(ds_unique_id, &ds_description))) {
                LOG_AND_TROW(CU_IDST_BC_ERR, err, boost::str(boost::format("Error fetchin description for data service %1%") % ds_unique_id));
            } else if(ds_description == NULL) {
                CU_IDST_BC_DBG << "No description foudn for data service:" << ds_unique_id;
            } else {
                std::auto_ptr<CDataWrapper> ds_object(ds_description);
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
    //sethte aciton type
    init_datapack->addInt32Value("action", (int32_t)0);
    return init_datapack;
}

std::auto_ptr<CDataWrapper> IDSTControlUnitBatchCommand::start(const std::string& cu_uid) {
    std::auto_ptr<CDataWrapper> result(new CDataWrapper());
    result->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, cu_uid);
    return result;
}

std::auto_ptr<CDataWrapper> IDSTControlUnitBatchCommand::stop(const std::string& cu_uid) {
    std::auto_ptr<CDataWrapper> result(new CDataWrapper());
    result->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, cu_uid);
    return result;
}

std::auto_ptr<CDataWrapper> IDSTControlUnitBatchCommand::deinitialize(const std::string& cu_uid) {
    
    int err = 0;
    CDataWrapper *tmp_ptr = NULL;
    
    std::auto_ptr<CDataWrapper> cu_base_description;
    
    //get default control unit node description
    if((err = getDataAccess<mds_data_access::NodeDataAccess>()->getNodeDescription(cu_uid, &tmp_ptr))) {
        LOG_AND_TROW(CU_IDST_BC_ERR, err, boost::str(boost::format("Error fetching the control unit node dafault description for unique id:%1% with error %2%") % cu_uid % err));
    } else if(tmp_ptr == NULL) {
        LOG_AND_TROW(CU_IDST_BC_ERR, err, boost::str(boost::format("No control unit node dafault description found for unique id:%1% ") % cu_uid));
    }
    cu_base_description.reset(tmp_ptr); tmp_ptr=NULL;
    
    //copy rpc information in the init datapack
    CHECK_KEY_THROW_AND_LOG(cu_base_description, NodeDefinitionKey::NODE_RPC_ADDR, CU_IDST_BC_ERR, -3, "No rpc addres in the control unit descirption")
    cu_base_description->addStringValue(NodeDefinitionKey::NODE_RPC_ADDR, cu_base_description->getStringValue(NodeDefinitionKey::NODE_RPC_ADDR));
    CHECK_KEY_THROW_AND_LOG(cu_base_description, NodeDefinitionKey::NODE_RPC_DOMAIN, CU_IDST_BC_ERR, -3, "No rpc domain in the control unit descirption")
    cu_base_description->addStringValue(NodeDefinitionKey::NODE_RPC_DOMAIN, cu_base_description->getStringValue(NodeDefinitionKey::NODE_RPC_DOMAIN));
    
    //sethte aciton type
    cu_base_description->addInt32Value("action", 3);
    //launch initilization in background
    return cu_base_description;
}