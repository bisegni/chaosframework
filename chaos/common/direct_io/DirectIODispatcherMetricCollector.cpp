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

using namespace chaos::common::direct_io;

static const char * const METRIC_KEY_ENDPOINT_ALIVE = "ndpoint_alive";

#define DIODMC_INFO INFO_LOG(DirectIODispatcherMetricCollector)
#define DIODMC_DBG_ DBG_LOG(DirectIODispatcherMetricCollector)
#define DIODMC_ERR_ ERR_LOG(DirectIODispatcherMetricCollector)

DirectIODispatcherMetricCollector::DirectIODispatcherMetricCollector(const std::string& direct_io_server_impl):
MetricCollectorIO(direct_io_server_impl,
                  GlobalConfiguration::getInstance()->getConfiguration()->getUInt64Value(InitOption::OPT_DIRECT_IO_LOG_METRIC_UPDATE_INTERVAL)),
endpoint_alive_count(0) {
    DIODMC_DBG_ << "Allcoate collector";
    //uppend custom direct io metric
    addMetric(METRIC_KEY_ENDPOINT_ALIVE, chaos::DataType::TYPE_INT32);
}

DirectIODispatcherMetricCollector::~DirectIODispatcherMetricCollector() {
    DIODMC_DBG_ << "Deallcoate collector";
}

// Start the implementation
void DirectIODispatcherMetricCollector::start() throw(chaos::CException) {
    //flow back to base class
    DirectIODispatcher::start();
    //start metric logging
    startLogging();
}

// Stop the implementation
void DirectIODispatcherMetricCollector::stop() throw(chaos::CException) {
    //stop metric logging
    stopLogging();
    //flow back to base class
    DirectIODispatcher::stop();
}

//! Allocate a new endpoint
DirectIOServerEndpoint *DirectIODispatcherMetricCollector::getNewEndpoint() {
    endpoint_alive_count++;
    return DirectIODispatcher::getNewEndpoint();
}

//! Relase the endpoint
void DirectIODispatcherMetricCollector::releaseEndpoint(DirectIOServerEndpoint *endpoint_to_release) {
    DirectIODispatcher::releaseEndpoint(endpoint_to_release);
    endpoint_alive_count--;
}

// Event for a new data received
int DirectIODispatcherMetricCollector::priorityDataReceived(chaos::common::direct_io::DirectIODataPackUPtr data_pack,
                                                            chaos::common::direct_io::DirectIODataPackSPtr& synchronous_answer) {
    //inrement packet count
    pack_count++;
    
    //increment packet size
    bandwith+=data_pack->header.channel_header_size+data_pack->header.channel_data_size + sizeof(DirectIODataPackDispatchHeader);

    //flow back to base class
    return DirectIODispatcher::priorityDataReceived(ChaosMoveOperator(data_pack),
                                                    synchronous_answer);
}

// Event for a new data received
int DirectIODispatcherMetricCollector::serviceDataReceived(chaos::common::direct_io::DirectIODataPackUPtr data_pack,
                                                           chaos::common::direct_io::DirectIODataPackSPtr& synchronous_answer) {
    //inrement packec count
    pack_count++;
    
    //increment packet size
    bandwith+=data_pack->header.channel_header_size+data_pack->header.channel_data_size + sizeof(DirectIODataPackDispatchHeader);

    //flow back to base class
    return DirectIODispatcher::serviceDataReceived(ChaosMoveOperator(data_pack),
                                                   synchronous_answer);
}

void DirectIODispatcherMetricCollector::fetchMetricForTimeDiff(uint64_t time_diff) {
    updateMetricValue(METRIC_KEY_ENDPOINT_ALIVE,
                      &endpoint_alive_count,
                      sizeof(int32_t));
    MetricCollectorIO::fetchMetricForTimeDiff(time_diff);
}
