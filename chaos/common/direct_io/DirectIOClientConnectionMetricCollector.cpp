/*
 *	DirectIOClientConnectionMetricCollector.cpp
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

#include <chaos/common/direct_io/DirectIOClientConnectionMetricCollector.h>

#include <chaos/common/configuration/GlobalConfiguration.h>

using namespace chaos::common::direct_io;

static const char * const METRIC_KEY_ENDPOINT_ALIVE = "ndpoint_alive";

#define DIOCCMC_INFO INFO_LOG(DirectIOClientConnectionMetricCollector)
#define DIOCCMC_DBG_ DBG_LOG(DirectIOClientConnectionMetricCollector)
#define DIOCCMC_ERR_ ERR_LOG(DirectIOClientConnectionMetricCollector)

DirectIOClientConnectionMetricCollector::DirectIOClientConnectionMetricCollector(const std::string& _server_description,
                                                                                 uint16_t _endpoint,
                                                                                 ChaosSharedPtr<DirectIOClientConnectionSharedMetricIO> _shared_collector,
                                                                                 DirectIOClientConnection *_wrapped_connection):
DirectIOClientConnection(_server_description,
                         _endpoint),
shared_collector(_shared_collector),
wrapped_connection(_wrapped_connection) {
    DIOCCMC_DBG_ << "Allocate collector";
}

DirectIOClientConnectionMetricCollector::~DirectIOClientConnectionMetricCollector() {
    DIOCCMC_DBG_ << "Deallocate collector";
}


//! inherited method
int64_t DirectIOClientConnectionMetricCollector::sendPriorityData(DirectIODataPack *data_pack,
                                                                  DirectIODeallocationHandler *header_deallocation_handler,
                                                                  DirectIODeallocationHandler *data_deallocation_handler,
                                                                  DirectIODataPack **asynchronous_answer) {
    CHAOS_ASSERT(wrapped_connection && shared_collector)
    //inrement packec count
    shared_collector->incrementPackCount();
    
    //increment packet size
    shared_collector->incrementBandWidth(data_pack->header.channel_header_size+data_pack->header.channel_data_size + sizeof(DirectIODataPack::DirectIODataPackDispatchHeader));
    return wrapped_connection->sendPriorityData(data_pack,
                                                header_deallocation_handler,
                                                data_deallocation_handler,
                                                asynchronous_answer);
}


//! inherited method
int64_t DirectIOClientConnectionMetricCollector::sendServiceData(DirectIODataPack *data_pack,
                                                                 DirectIODeallocationHandler *header_deallocation_handler,
                                                                 DirectIODeallocationHandler *data_deallocation_handler,
                                                                 DirectIODataPack **asynchronous_answer) {
    CHAOS_ASSERT(wrapped_connection && shared_collector)
    //inrement packec count
    shared_collector->incrementPackCount();
    
    //increment packet size
    shared_collector->incrementBandWidth(data_pack->header.channel_header_size+data_pack->header.channel_data_size + sizeof(DirectIODataPack::DirectIODataPackDispatchHeader));
    return wrapped_connection->sendServiceData(data_pack,
                                               header_deallocation_handler,
                                               data_deallocation_handler,
                                               asynchronous_answer);
}
