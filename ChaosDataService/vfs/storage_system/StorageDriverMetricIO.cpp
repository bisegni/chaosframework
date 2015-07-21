/*
 *	StorageDriverMetricIO.cpp
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

#include "StorageDriverMetricIO.h"
#include "../../ChaosDataService.h"

#include <boost/format.hpp>

using namespace chaos::data_service::vfs::storage_system;

#define CACHDRVMC_INFO INFO_LOG(CacheDriverSharedMetricIO)
#define CACHDRVMC_DBG_ DBG_LOG(CacheDriverSharedMetricIO)
#define CACHDRVMC_ERR_ ERR_LOG(CacheDriverSharedMetricIO)

static const char * const METRIC_KEY_WRITE_PACKET_COUNT = "w_packet_count";
static const char * const METRIC_KEY_WRITE_BANDWITH = "w_kb_sec";
static const char * const METRIC_KEY_READ_PACKET_COUNT = "r_packet_count";
static const char * const METRIC_KEY_READ_BANDWITH = "r_kb_sec";
static const char * const METRIC_KEY_FLUSH_COUNT = "flush_count";
StorageDriverMetricIO::StorageDriverMetricIO(const std::string& client_impl):
MetricCollectorIO(client_impl,
                  ChaosDataService::getInstance()->setting.cache_driver_setting.log_metric_update_interval),
wr_count(0),
wr_bandwith(0),
rd_count(0),
rd_bandwith(0),
flush_count(0) {
    //add custom key
    addMetric(METRIC_KEY_WRITE_PACKET_COUNT, chaos::DataType::TYPE_DOUBLE);
    addMetric(METRIC_KEY_WRITE_BANDWITH, chaos::DataType::TYPE_DOUBLE);
    addMetric(METRIC_KEY_READ_PACKET_COUNT, chaos::DataType::TYPE_DOUBLE);
    addMetric(METRIC_KEY_READ_BANDWITH, chaos::DataType::TYPE_DOUBLE);
    addMetric(METRIC_KEY_FLUSH_COUNT, chaos::DataType::TYPE_DOUBLE);
    
    //add backend
    addBackend(common::metric::MetricBackendPointer(new common::metric::ConsoleMetricBackend(client_impl)));
}

StorageDriverMetricIO::~StorageDriverMetricIO() {
}

void StorageDriverMetricIO::updateWriteBandwith(uint32_t badnwith) {
    boost::shared_lock<boost::shared_mutex> rl(metric_mutex);
    wr_count++;
    wr_bandwith += badnwith;
}

void StorageDriverMetricIO::updateReadBandwith(uint32_t badnwith) {
    boost::shared_lock<boost::shared_mutex> rl(metric_mutex);
    rd_count++;
    rd_bandwith += badnwith;
}

void StorageDriverMetricIO::incrementFlushCall() {
    boost::shared_lock<boost::shared_mutex> rl(metric_mutex);
    flush_count++;
}

void StorageDriverMetricIO::fetchMetricForTimeDiff(uint64_t time_diff) {
    double sec = time_diff/1000;
    boost::unique_lock<boost::shared_mutex> wl(metric_mutex);
    //update total
    pack_count = wr_count+rd_count;
    bandwith = wr_bandwith + rd_bandwith;
    
    
    //udpate partial
    double wr_pc = wr_count/sec; wr_count = 0;
    double wr_bw = ((wr_bandwith/sec)/1024); wr_bandwith = 0;
    double rd_pc = rd_count/sec; rd_count = 0;
    double rd_bw = ((rd_bandwith/sec)/1024); rd_bandwith = 0;
    double f_count = flush_count/sec; flush_count = 0;
    
    //update local variable cache
    updateMetricValue(METRIC_KEY_WRITE_PACKET_COUNT,
                      &wr_pc,
                      sizeof(double));
    updateMetricValue(METRIC_KEY_WRITE_BANDWITH,
                      &wr_bw,
                      sizeof(double));
    updateMetricValue(METRIC_KEY_READ_PACKET_COUNT,
                      &rd_pc,
                      sizeof(double));
    updateMetricValue(METRIC_KEY_READ_BANDWITH,
                      &rd_bw,
                      sizeof(double));
    updateMetricValue(METRIC_KEY_FLUSH_COUNT,
                      &f_count,
                      sizeof(double));
    MetricCollectorIO::fetchMetricForTimeDiff(time_diff);
}
