/*
 *	DeviceSharedDataWorkerMetric.cpp
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2014 INFN, National Institute of Nuclear Physics
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

#include "DeviceSharedDataWorkerMetric.h"

using namespace chaos::data_service::worker;

static const char * const METRIC_KEY_INPUT_PACKET_COUNT = "input_packet_count";
static const char * const METRIC_KEY_INPUT_BANDWITH = "input_badnwith_kb";
static const char * const METRIC_KEY_OUTPUT_PACKET_COUNT = "output_packet_count";
static const char * const METRIC_KEY_OUTPUT_BADNWITH = "output_bandwith_kb";
static const char * const METRIC_KEY_QUEUED_PACKET_COUNT = "queued_packet_count";
static const char * const METRIC_KEY_QUEUED_PACKET_SIZE = "queued_packet_size_kb";

DeviceSharedDataWorkerMetric::DeviceSharedDataWorkerMetric(const std::string& _collector_name,
                                                           uint64_t update_time_in_sec):
MetricCollector(_collector_name,
                update_time_in_sec),
input_data_pack(0),
input_badnwith(0),
output_data_pack(0),
output_badnwith(0),
queued_data_pack(0),
queued_size(0){
    //received pack and bw in the
    addMetric(METRIC_KEY_INPUT_PACKET_COUNT, chaos::DataType::TYPE_DOUBLE);
    addMetric(METRIC_KEY_INPUT_BANDWITH, chaos::DataType::TYPE_DOUBLE);
    addMetric(METRIC_KEY_OUTPUT_PACKET_COUNT, chaos::DataType::TYPE_DOUBLE);
    addMetric(METRIC_KEY_OUTPUT_BADNWITH, chaos::DataType::TYPE_DOUBLE);
    addMetric(METRIC_KEY_QUEUED_PACKET_COUNT, chaos::DataType::TYPE_INT64);
    addMetric(METRIC_KEY_QUEUED_PACKET_SIZE, chaos::DataType::TYPE_INT64);

    startLogging();
}

DeviceSharedDataWorkerMetric::~DeviceSharedDataWorkerMetric() {
    stopLogging();
}


void DeviceSharedDataWorkerMetric::incrementInputBandwith(uint32_t pack_size) {
    boost::shared_lock<boost::shared_mutex> rl(metric_mutex);
    input_data_pack++;
    input_badnwith+=pack_size;
}

void DeviceSharedDataWorkerMetric::incrementOutputBandwith(uint32_t pack_size) {
    boost::shared_lock<boost::shared_mutex> rl(metric_mutex);
    output_data_pack++;
    output_badnwith+=pack_size;
}

void DeviceSharedDataWorkerMetric::incrementQueueSize(uint32_t pack_size) {
    boost::shared_lock<boost::shared_mutex> rl(metric_mutex);
    queued_data_pack++;
    queued_size += pack_size;
}

void DeviceSharedDataWorkerMetric::decrementQueueSize(uint32_t pack_size) {
    boost::shared_lock<boost::shared_mutex> rl(metric_mutex);
    queued_data_pack--;
    queued_size -= pack_size;
}

void DeviceSharedDataWorkerMetric::fetchMetricForTimeDiff(uint64_t time_diff) {
    boost::unique_lock<boost::shared_mutex> wl(metric_mutex);

    double sec = time_diff/1000;
    //udpate partial
    double in_pc = input_data_pack/sec; input_data_pack = 0;
    double in_bw = ((input_badnwith/sec)/1024); input_badnwith = 0;
    double out_pc = output_data_pack/sec; output_data_pack = 0;
    double out_bw = ((output_badnwith/sec)/1024); output_badnwith = 0;
    int64_t q_pc = queued_data_pack;
    int64_t q_size = ((queued_size)/1024);

    //update local variable cache
    updateMetricValue(METRIC_KEY_INPUT_PACKET_COUNT,
                      &in_pc,
                      sizeof(double));
    updateMetricValue(METRIC_KEY_INPUT_BANDWITH,
                      &in_bw,
                      sizeof(double));
    updateMetricValue(METRIC_KEY_OUTPUT_PACKET_COUNT,
                      &out_pc,
                      sizeof(double));
    updateMetricValue(METRIC_KEY_OUTPUT_BADNWITH,
                      &out_bw,
                      sizeof(double));
    updateMetricValue(METRIC_KEY_QUEUED_PACKET_COUNT,
                      &q_pc,
                      sizeof(int64_t));
    updateMetricValue(METRIC_KEY_QUEUED_PACKET_SIZE,
                      &q_size,
                      sizeof(int64_t));
}
