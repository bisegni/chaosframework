/*
 *	MetricCollector.cpp
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

#include <chaos/common/metric/MetricCollectorIO.h>

using namespace chaos::common::metric;

static const char * const METRIC_KEY_PACKET_COUNT = "packet_count";
static const char * const METRIC_KEY_BANDWITH = "kb_sec";
//static const char * const METRIC_KEY_PACKET_UNSENT_COUNT = "packet_waiting";

MetricCollectorIO::MetricCollectorIO(const std::string& _collector_name,
                                     uint64_t update_time_in_sec):
MetricCollector(_collector_name,
                update_time_in_sec),
pack_count(0),
bandwith(0),
pack_count_for_ut(0.0),
bw_for_ut(0.0){
    //received pack and bw in the
    addMetric(METRIC_KEY_PACKET_COUNT, chaos::DataType::TYPE_DOUBLE);
    addMetric(METRIC_KEY_BANDWITH, chaos::DataType::TYPE_DOUBLE);
}

MetricCollectorIO::~MetricCollectorIO() {
    
}

void MetricCollectorIO::fetchMetricForTimeDiff(uint64_t time_diff) {
    double sec = time_diff/1000;
    if(sec > 0) {
        pack_count_for_ut = pack_count / sec; pack_count = 0;
        bw_for_ut = ((bandwith / sec)/1024); bandwith = 0;
        
        updateMetricValue(METRIC_KEY_PACKET_COUNT,
                          &pack_count_for_ut,
                          sizeof(double));
        updateMetricValue(METRIC_KEY_BANDWITH,
                          &bw_for_ut,
                          sizeof(double));
    }
}
