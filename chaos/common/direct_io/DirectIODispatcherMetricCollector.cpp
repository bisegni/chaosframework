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

#include <chaos/common/direct_io/DirectIODispatcherMetricCollector.h>

#include <chaos/common/configuration/GlobalConfiguration.h>

using namespace chaos::common::metric;
using namespace chaos::common::direct_io;

static const char * const METRIC_KEY_ENDPOINT_ALIVE = "ndpoint_alive";

#define DIODMC_INFO INFO_LOG(DirectIODispatcherMetricCollector)
#define DIODMC_DBG_ DBG_LOG(DirectIODispatcherMetricCollector)
#define DIODMC_ERR_ ERR_LOG(DirectIODispatcherMetricCollector)

DirectIODispatcherMetricCollector::DirectIODispatcherMetricCollector():
MetricCollectorIO(),
endpoint_alive_count(0),
current_bandwidth(0){
    DIODMC_DBG_ << "Allcoate collector";
    //uppend custom direct io metric
    coutenr_pack_uptr = MetricManager::getInstance()->getNewRxPacketRateMetricFamily({{"driver","direct_io"}});
    counter_data_uptr = MetricManager::getInstance()->getNewRxDataRateMetricFamily({{"driver","direct_io"}});
    
    MetricManager::getInstance()->createGaugeFamily("rx_bandwidth_direct_io_dispatcher", "Metric for bandwith measure on data received by direct-io dispatcher");
    gauge_bandwidth_uptr = MetricManager::getInstance()->getNewGaugeFromFamily("rx_bandwidth_direct_io_dispatcher");
}

DirectIODispatcherMetricCollector::~DirectIODispatcherMetricCollector() {
    DIODMC_DBG_ << "Deallcoate collector";
}

// Start the implementation
void DirectIODispatcherMetricCollector::start()  {
    //flow back to base class
    DirectIODispatcher::start();
    //start metric logging
    startLogging();
}

// Stop the implementation
void DirectIODispatcherMetricCollector::stop()  {
    //stop metric logging
    stopLogging();
    //flow back to base class
    DirectIODispatcher::stop();
}

//! Allocate a new endpoint
DirectIOServerEndpoint *DirectIODispatcherMetricCollector::getNewEndpoint() {
    return DirectIODispatcher::getNewEndpoint();
}

//! Relase the endpoint
void DirectIODispatcherMetricCollector::releaseEndpoint(DirectIOServerEndpoint *endpoint_to_release) {
    DirectIODispatcher::releaseEndpoint(endpoint_to_release);
}

// Event for a new data received
int DirectIODispatcherMetricCollector::priorityDataReceived(chaos::common::direct_io::DirectIODataPackSPtr data_pack,
                                                            chaos::common::direct_io::DirectIODataPackSPtr& synchronous_answer) {
    //inrement packet count
    (*coutenr_pack_uptr)++;
    
    //increment packet size
    long total_data = data_pack->header.channel_header_size+data_pack->header.channel_data_size + sizeof(DirectIODataPackDispatchHeader);
    current_bandwidth += total_data;
    (*counter_data_uptr)+= total_data;

    //flow back to base class
    return DirectIODispatcher::priorityDataReceived(MOVE(data_pack),
                                                    synchronous_answer);
}

// Event for a new data received
int DirectIODispatcherMetricCollector::serviceDataReceived(chaos::common::direct_io::DirectIODataPackSPtr data_pack,
                                                           chaos::common::direct_io::DirectIODataPackSPtr& synchronous_answer) {
    //inrement packec count
    (*coutenr_pack_uptr)++;
    
    //increment packet size
    long total_data = data_pack->header.channel_header_size+data_pack->header.channel_data_size + sizeof(DirectIODataPackDispatchHeader);
    current_bandwidth += total_data;
    (*counter_data_uptr)+= total_data;

    //flow back to base class
    return DirectIODispatcher::serviceDataReceived(MOVE(data_pack),
                                                   synchronous_answer);
}

void DirectIODispatcherMetricCollector::fetchMetricForTimeDiff(uint64_t time_diff) {
    double sec = time_diff/1000;
    if(sec == 0) return;
    (*gauge_bandwidth_uptr) = ((current_bandwidth / sec)/1024); current_bandwidth = 0;
}
