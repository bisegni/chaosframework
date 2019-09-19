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

using namespace chaos::common::metric;
using namespace chaos::common::direct_io;

#define DIOCCMC_INFO INFO_LOG(DirectIOClientConnectionMetricCollector)
#define DIOCCMC_DBG_ DBG_LOG(DirectIOClientConnectionMetricCollector)
#define DIOCCMC_ERR_ ERR_LOG(DirectIOClientConnectionMetricCollector)

DirectIOClientConnectionMetricCollector::DirectIOClientConnectionMetricCollector(const std::string& _server_description,
                                                                                 uint16_t _endpoint,
                                                                                 DirectIOClientConnection *_wrapped_connection):
DirectIOClientConnection(_server_description,
                         _endpoint),
MetricCollectorIO(),
wrapped_connection(_wrapped_connection),
current_bandwidth(0) {
    DIOCCMC_DBG_ << "Allocate collector";
    counter_dataseet_sent = MetricManager::getInstance()->getNewTxPacketRateMetricFamily({{"driver","direct_io"}});
    counter_data_sent = MetricManager::getInstance()->getNewTxDataRateMetricFamily({{"driver","direct_io"}});
    
    MetricManager::getInstance()->createGaugeFamily("tx_bandwidth_direct_io_dispatcher", "Metric for bandwith measure on data transmitted by direct-io dispatcher [byte]");
    gauge_bandwidth_uptr = MetricManager::getInstance()->getNewGaugeFromFamily("tx_bandwidth_direct_io_dispatcher");
    
    startLogging();
}

DirectIOClientConnectionMetricCollector::~DirectIOClientConnectionMetricCollector() {
    DIOCCMC_DBG_ << "Deallocate collector";
    stopLogging();
}


//! inherited method
int DirectIOClientConnectionMetricCollector::sendPriorityData(DirectIODataPackSPtr data_pack) {
    CHAOS_ASSERT(wrapped_connection)
    //inrement packec count
    (*counter_dataseet_sent)++;
    
    //increment packet size
    int32_t total_data = (data_pack->header.channel_header_size+data_pack->header.channel_data_size + sizeof(DirectIODataPackDispatchHeader));
    current_bandwidth += total_data;
    (*counter_data_sent) += total_data;
    return wrapped_connection->sendPriorityData(MOVE(data_pack));
}
int DirectIOClientConnectionMetricCollector::sendPriorityData(DirectIODataPackSPtr data_pack,
                                                              DirectIODataPackSPtr& asynchronous_answer) {
    CHAOS_ASSERT(wrapped_connection)
    //inrement packec count
    (*counter_dataseet_sent)++;
    
    //increment packet size
    int32_t total_data = (data_pack->header.channel_header_size+data_pack->header.channel_data_size + sizeof(DirectIODataPackDispatchHeader));
    current_bandwidth += total_data;
    (*counter_data_sent) += total_data;
    return wrapped_connection->sendPriorityData(MOVE(data_pack),
                                                asynchronous_answer);
}


//! inherited method
int DirectIOClientConnectionMetricCollector::sendServiceData(chaos::common::direct_io::DirectIODataPackSPtr data_pack) {
    CHAOS_ASSERT(wrapped_connection)
    //inrement packec count
    (*counter_dataseet_sent)++;
    
    //increment packet size
    int32_t total_data = (data_pack->header.channel_header_size+data_pack->header.channel_data_size + sizeof(DirectIODataPackDispatchHeader));
    current_bandwidth += total_data;
    (*counter_data_sent) += total_data;
    return wrapped_connection->sendServiceData(MOVE(data_pack));
}
int DirectIOClientConnectionMetricCollector::sendServiceData(chaos::common::direct_io::DirectIODataPackSPtr data_pack,
                                                             chaos::common::direct_io::DirectIODataPackSPtr& asynchronous_answer) {
    CHAOS_ASSERT(wrapped_connection)
    //inrement packec count
    (*counter_dataseet_sent)++;
    
    //increment packet size
    int32_t total_data = (data_pack->header.channel_header_size+data_pack->header.channel_data_size + sizeof(DirectIODataPackDispatchHeader));
    current_bandwidth += total_data;
    (*counter_data_sent) += total_data;
    return wrapped_connection->sendServiceData(MOVE(data_pack),
                                               asynchronous_answer);
}

void DirectIOClientConnectionMetricCollector::fetchMetricForTimeDiff(uint64_t time_diff) {
    double sec = time_diff/1000;
    if(sec == 0) return;
    (*gauge_bandwidth_uptr) = (current_bandwidth / sec); current_bandwidth = 0;
}
