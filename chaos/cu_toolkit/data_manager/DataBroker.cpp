/*
 *	DataBroker.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 25/07/16 INFN, National Institute of Nuclear Physics
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

#include <chaos/cu_toolkit/data_manager/DataBroker.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::direct_io::channel::opcode_headers;

using namespace chaos::cu::data_manager;
using namespace chaos::cu::data_manager::publishing;
using namespace chaos::cu::data_manager::manipulation;
using namespace chaos::cu::data_manager::trigger_system;

#pragma mark DataBroker
DataBroker::DataBroker():
dataset_manager(),
publishing_manager(dataset_manager.container_dataset),
event_manager(dataset_manager.container_dataset){}

DataBroker::~DataBroker() {}

void DataBroker::init(void* init_data) throw(CException) {
    
}

void DataBroker::deinit() throw(CException) {
    
}

void DataBroker::getDeclaredActionInstance(std::vector<const DeclareAction *>& declared_action_list) {
    
}

int DataBroker::addNewDataset(const std::string& name,
                              const DataType::DatasetType type,
                              const std::string& shared_key) {
    return dataset_manager.addNewDataset(name,
                                         type,
                                         shared_key);
}

std::auto_ptr<DatasetEditor> DataBroker::getEditorForDataset(const std::string& dataset_name) {
    if(dataset_manager.hasDataset(dataset_name) == false){
        //we need to creare the dataset because it it not present
        return std::auto_ptr<DatasetEditor>();
    }
    return dataset_manager.getDatasetEditorFor(dataset_name);
}

std::auto_ptr<manipulation::DatasetCacheWrapper> DataBroker::getDatasetCacheForDataset(const std::string& dataset_name) {
    return dataset_manager.getDatasetCacheWrapperFor(dataset_name);
}

std::auto_ptr<CDataWrapper> DataBroker::serialize() {
    std::auto_ptr<CDataWrapper> result(new CDataWrapper());
    std::auto_ptr<CDataWrapper>  tmp_ser = dataset_manager.serialize();
    if(tmp_ser.get()) {
        result->addCSDataValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION,  *tmp_ser);
    }
    tmp_ser = event_manager.serialize();
    if(tmp_ser.get()) {
        result->addCSDataValue("dataset_trigger",  *tmp_ser);
    }
    return result;
}

void DataBroker::deserialize(std::auto_ptr<chaos::common::data::CDataWrapper> data_serailization) {
    if(data_serailization.get() == NULL) return;
    //check if we have dataset serialized data
    if(data_serailization->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION) &&
       data_serailization->isCDataWrapperValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION)){
        std::auto_ptr<chaos::common::data::CDataWrapper> dataset_serialization(data_serailization->getCSDataValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION));
        dataset_manager.deserialize(*dataset_serialization);
    }
}

#pragma mark DataBroker Protected Method
int DataBroker::consumePutEvent(DirectIODeviceChannelHeaderPutOpcode *header,
                                void *channel_data,
                                uint32_t channel_data_len) {
    return -1;
}

int DataBroker::consumeGetEvent(DirectIODeviceChannelHeaderGetOpcode *header,
                                void *channel_data,
                                uint32_t channel_data_len,
                                DirectIODeviceChannelHeaderGetOpcodeResult *result_header,
                                void **result_value) {
    return -1;
}

int DataBroker::consumeDataCloudQuery(DirectIODeviceChannelHeaderOpcodeQueryDataCloud *header,
                                      const std::string& search_key,
                                      uint64_t search_start_ts,
                                      uint64_t search_end_ts) {
    return -1;
}
