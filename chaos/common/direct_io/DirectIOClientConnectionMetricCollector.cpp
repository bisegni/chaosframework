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

#include <chaos/common/direct_io/DirectIOClientConnectionMetricCollector.h>

#include <chaos/common/configuration/GlobalConfiguration.h>

using namespace chaos::common::direct_io;

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
int DirectIOClientConnectionMetricCollector::sendPriorityData(DirectIODataPackSPtr data_pack) {
    CHAOS_ASSERT(wrapped_connection && shared_collector)
    //inrement packec count
    shared_collector->incrementPackCount();
    
    //increment packet size
    shared_collector->incrementBandWidth(data_pack->header.channel_header_size+data_pack->header.channel_data_size + sizeof(DirectIODataPackDispatchHeader));
    return wrapped_connection->sendPriorityData(MOVE(data_pack));
}
int DirectIOClientConnectionMetricCollector::sendPriorityData(DirectIODataPackSPtr data_pack,
                                                              DirectIODataPackSPtr& asynchronous_answer) {
    CHAOS_ASSERT(wrapped_connection && shared_collector)
    //inrement packec count
    shared_collector->incrementPackCount();
    
    //increment packet size
    shared_collector->incrementBandWidth(data_pack->header.channel_header_size+data_pack->header.channel_data_size + sizeof(DirectIODataPackDispatchHeader));
    return wrapped_connection->sendPriorityData(MOVE(data_pack),
                                                asynchronous_answer);
}


//! inherited method
int DirectIOClientConnectionMetricCollector::sendServiceData(chaos::common::direct_io::DirectIODataPackSPtr data_pack) {
    CHAOS_ASSERT(wrapped_connection && shared_collector)
    //inrement packec count
    shared_collector->incrementPackCount();
    
    //increment packet size
    shared_collector->incrementBandWidth(data_pack->header.channel_header_size+data_pack->header.channel_data_size + sizeof(DirectIODataPackDispatchHeader));
    return wrapped_connection->sendServiceData(MOVE(data_pack));
}
int DirectIOClientConnectionMetricCollector::sendServiceData(chaos::common::direct_io::DirectIODataPackSPtr data_pack,
                                                             chaos::common::direct_io::DirectIODataPackSPtr& asynchronous_answer) {
    CHAOS_ASSERT(wrapped_connection && shared_collector)
    //inrement packec count
    shared_collector->incrementPackCount();
    
    //increment packet size
    shared_collector->incrementBandWidth(data_pack->header.channel_header_size+data_pack->header.channel_data_size + sizeof(DirectIODataPackDispatchHeader));
    return wrapped_connection->sendServiceData(MOVE(data_pack),
                                               asynchronous_answer);
}
