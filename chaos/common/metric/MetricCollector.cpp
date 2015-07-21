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

#include <chaos/common/metric/MetricCollector.h>
#include <chaos/common/utility/TimingUtil.h>

#include <sstream>

#define MC_INFO INFO_LOG(MetricCollector)
#define MC_LDBG DBG_LOG(MetricCollector)
#define MC_LERR ERR_LOG(MetricCollector)

using namespace chaos::common::metric;
using namespace chaos::common::data::cache;
using namespace chaos::common::async_central;

MetricCollector::MetricCollector(const std::string& _collector_name,
                                 uint64_t update_time_in_sec):
collector_name(_collector_name),
current_slot_index(0),
last_stat_call(0),
stat_intervall(update_time_in_sec*1000){
    caching_slot[1].metric_attribute_cache.init(NULL);
    caching_slot[0].metric_attribute_cache.init(NULL);
}

MetricCollector::~MetricCollector() {
    stopLogging();
    caching_slot[1].metric_attribute_cache.deinit();
    caching_slot[0].metric_attribute_cache.deinit();
}

void MetricCollector::addBackend(MetricBackendPointer backend) {
    vector_metric_backend.push_back(backend);
}

bool MetricCollector::_addMetric(ChachingSlot& cs,
                                 const std::string& metric_name,
                                 chaos::DataType::DataType metric_type,
                                 uint32_t metric_max_size) {
    cs.metric_attribute_cache.addAttribute(metric_name, metric_max_size, metric_type);
    AttributeValue *cached_metric = cs.metric_attribute_cache.getValueSettingForIndex(cs.metric_attribute_cache.getIndexForName(metric_name));
    if(cached_metric) {
        cs.map_attribute_value.insert(make_pair(metric_name, cached_metric));
        return true;
    } else {
        return false;
    }
    
}

bool MetricCollector::addMetric(const std::string& metric_name,
                                chaos::DataType::DataType metric_type,
                                uint32_t metric_max_size) {
    boost::shared_lock<boost::shared_mutex> rl(current_slot_index_mutex);
    return _addMetric(caching_slot[0], metric_name, metric_type, metric_max_size) &&
    _addMetric(caching_slot[1], metric_name, metric_type, metric_max_size);
}

bool MetricCollector::updateMetricValue(const std::string& metric_name,
                                        const void *value_ptr,
                                        uint32_t value_size) {
    boost::shared_lock<boost::shared_mutex> rl(current_slot_index_mutex);
    if(caching_slot[current_slot_index].map_attribute_value.count(metric_name) == 0) return false;
    return caching_slot[current_slot_index].map_attribute_value[metric_name]->setValue(value_ptr,
                                                                                        value_size);
}

void MetricCollector::writeTo(chaos::common::data::CDataWrapper& data_wrapper) {
    //switch slot
    boost::unique_lock<boost::shared_mutex> rwl(current_slot_index_mutex);
    ChachingSlot *slot_to_persist =  &caching_slot[(current_slot_index ^= 0x00000001)];
    rwl.unlock();
    
    //persiste previous slot
    for(MapMetricIterator it = slot_to_persist->map_attribute_value.begin();
        it != slot_to_persist->map_attribute_value.end();
        it++) {
        it->second->writeToCDataWrapper(data_wrapper);
    }
}

void MetricCollector::timeout() {
    if(last_stat_call) {
        fetchMetricForTimeDiff(chaos::common::utility::TimingUtil::getTimeStamp() - last_stat_call);
    } else {
        fetchMetricForTimeDiff(0);
    }
    last_stat_call = chaos::common::utility::TimingUtil::getTimeStamp();
    //switch slot
    boost::unique_lock<boost::shared_mutex> rwl(current_slot_index_mutex);
    ChachingSlot *slot_to_persist =  &caching_slot[(current_slot_index ^= 0x00000001) ^ 0x00000001];
    rwl.unlock();
    
    //write metric to backend
    CHAOS_SCAN_VECTOR_ITERATOR(VectorMetricBackendIterator, vector_metric_backend, (*it)->prepare();)
    for(MapMetricIterator it_metric = slot_to_persist->map_attribute_value.begin();
        it_metric != slot_to_persist->map_attribute_value.end();
        it_metric++) {
        CHAOS_SCAN_VECTOR_ITERATOR(VectorMetricBackendIterator,
                                   vector_metric_backend,
                                   (*it)->addMetric(it_metric->second->name,
                                                    it_metric->second->toString());)
    }
    CHAOS_SCAN_VECTOR_ITERATOR(VectorMetricBackendIterator, vector_metric_backend, (*it)->flush();)
}

void MetricCollector::setStatIntervalInSeconds(uint64_t stat_intervall_in_seconds) {
    stat_intervall = stat_intervall_in_seconds*1000;
 }

void MetricCollector::startLogging() {
    last_stat_call = chaos::common::utility::TimingUtil::getTimeStamp();
    AsyncCentralManager::getInstance()->addTimer(this, 1000, stat_intervall);
}

void MetricCollector::stopLogging() {
    AsyncCentralManager::getInstance()->removeTimer(this);
}