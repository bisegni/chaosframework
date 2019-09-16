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

#include <chaos/common/metric/MetricCollectorIO.h>

#include <chaos/common/utility/TimingUtil.h>

using namespace chaos::common::metric;
using namespace chaos::common::async_central;

MetricCollectorIO::MetricCollectorIO():
MetricCollector(),
last_sample_ts(0){
    //received pack and bw in the
//    addMetric(METRIC_KEY_PACKET_COUNT, chaos::DataType::TYPE_DOUBLE);
//    addMetric(METRIC_KEY_BANDWITH, chaos::DataType::TYPE_DOUBLE);
    last_sample_ts = chaos::common::utility::TimingUtil::getTimeStamp();
}

MetricCollectorIO::~MetricCollectorIO() {
    
}

void MetricCollectorIO::startLogging() {
    AsyncCentralManager::getInstance()->addTimer(this, chaos::common::constants::MetricCollectorTimeoutinMSec, chaos::common::constants::MetricCollectorTimeoutinMSec);
}

void MetricCollectorIO::stopLogging() {
    AsyncCentralManager::getInstance()->removeTimer(this);
}

//void MetricCollectorIO::fetchMetricForTimeDiff(uint64_t time_diff) {
//    double sec = time_diff/1000;
//    if(sec > 0) {
//        pack_count_for_ut = pack_count / sec; pack_count = 0;
//        bw_for_ut = ((bandwith / sec)/1024); bandwith = 0;
//        
//        updateMetricValue(METRIC_KEY_PACKET_COUNT,
//                          &pack_count_for_ut,
//                          sizeof(double));
//        updateMetricValue(METRIC_KEY_BANDWITH,
//                          &bw_for_ut,
//                          sizeof(double));
//    }
//}

void MetricCollectorIO::timeout() {
    uint64_t now = chaos::common::utility::TimingUtil::getTimeStamp();
    if(last_sample_ts) {
        fetchMetricForTimeDiff(now - last_sample_ts);
    } else {
        fetchMetricForTimeDiff(0);
    }
    last_sample_ts = now;
}
