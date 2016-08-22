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
using namespace chaos::cu::data_manager;
using namespace chaos::cu::data_manager::publishing;
using namespace chaos::cu::data_manager::manipulation;
using namespace chaos::cu::data_manager::trigger_system;

using namespace chaos::common::direct_io::channel::opcode_headers;
#pragma mark DataBroker
DataBroker::DataBroker():
dataset_manager(),
publishing_manager(dataset_manager.container_dataset){}

DataBroker::~DataBroker() {}

void DataBroker::init(void* init_data) throw(CException) {
    
}

void DataBroker::deinit() throw(CException) {
    
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
