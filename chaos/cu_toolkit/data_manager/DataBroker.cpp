/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */

#include <chaos/cu_toolkit/data_manager/DataBroker.h>

using namespace chaos;
using namespace chaos::cu::data_manager;
using namespace chaos::cu::data_manager::publishing;
using namespace chaos::cu::data_manager::manipulation;
using namespace chaos::cu::data_manager::trigger_system;

using namespace chaos::common::direct_io::channel::opcode_headers;
#pragma mark DataBroker
DataBroker::DataBroker(const std::string& _owner_node_uid):
owner_node_uid(_owner_node_uid),
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
