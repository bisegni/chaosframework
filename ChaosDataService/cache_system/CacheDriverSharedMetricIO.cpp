/*
 *	CacheDriverSharedMetricIO.cpp
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

#include "CacheDriverSharedMetricIO.h"
#include "../ChaosDataService.h"

#include <boost/format.hpp>

using namespace chaos::data_service::cache_system;

#define CACHDRVMC_INFO INFO_LOG(CacheDriverSharedMetricIO)
#define CACHDRVMC_DBG_ DBG_LOG(CacheDriverSharedMetricIO)
#define CACHDRVMC_ERR_ ERR_LOG(CacheDriverSharedMetricIO)

static const char * const METRIC_KEY_SET_PACKET_COUNT = "set_packet_count";
static const char * const METRIC_KEY_SET_BANDWITH = "set_kb_sec";
static const char * const METRIC_KEY_GET_PACKET_COUNT = "get_packet_count";
static const char * const METRIC_KEY_GET_BANDWITH = "get_kb_sec";
CacheDriverSharedMetricIO::CacheDriverSharedMetricIO(const std::string& client_impl):
MetricCollectorIO(client_impl,
                  ChaosDataService::getInstance()->setting.cache_driver_setting.log_metric_update_interval),
set_pack_count(0),
set_bandwith(0),
get_pack_count(0),
get_bandwith(0) {

    CACHDRVMC_DBG_ << "Allocate collector";
    //add custom key
    addMetric(METRIC_KEY_SET_PACKET_COUNT, chaos::DataType::TYPE_DOUBLE);
    addMetric(METRIC_KEY_SET_BANDWITH, chaos::DataType::TYPE_DOUBLE);
    addMetric(METRIC_KEY_GET_PACKET_COUNT, chaos::DataType::TYPE_DOUBLE);
    addMetric(METRIC_KEY_GET_BANDWITH, chaos::DataType::TYPE_DOUBLE);
    //start logging
    startLogging();
}

CacheDriverSharedMetricIO::~CacheDriverSharedMetricIO() {
    //stop logging
    stopLogging();
}

void CacheDriverSharedMetricIO::incrementSetBandWidth(uint32_t increment) {
    boost::shared_lock<boost::shared_mutex> rl(metric_mutex);
    set_pack_count++;
    set_bandwith += increment;
}

void CacheDriverSharedMetricIO::incrementGetBandWidth(uint32_t increment) {
    boost::shared_lock<boost::shared_mutex> rl(metric_mutex);
    get_pack_count++;
    get_bandwith+=increment;
}

void CacheDriverSharedMetricIO::fetchMetricForTimeDiff(uint64_t time_diff) {
    double sec = time_diff/1000;
    boost::unique_lock<boost::shared_mutex> wl(metric_mutex);
    //update total
    pack_count = set_pack_count+get_pack_count;
    bandwith = set_bandwith + get_bandwith;


    //udpate partial
    double set_pc = set_pack_count/sec; set_pack_count = 0;
    double set_bw = ((set_bandwith/sec)/1024); set_bandwith = 0;
    double get_pc = get_pack_count/sec; get_pack_count = 0;
    double get_bw = ((get_bandwith/sec)/1024); get_bandwith = 0;

    //update local variable cache
    updateMetricValue(METRIC_KEY_SET_PACKET_COUNT,
                      &set_pc,
                      sizeof(double));
    updateMetricValue(METRIC_KEY_SET_BANDWITH,
                      &set_bw,
                      sizeof(double));
    updateMetricValue(METRIC_KEY_GET_PACKET_COUNT,
                      &get_pc,
                      sizeof(double));
    updateMetricValue(METRIC_KEY_GET_BANDWITH,
                      &get_bw,
                      sizeof(double));
    //call sublcass
    MetricCollectorIO::fetchMetricForTimeDiff(time_diff);
}
