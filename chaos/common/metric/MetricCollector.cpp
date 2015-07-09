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

using namespace chaos::common::metric;
using namespace chaos::common::data::cache;
using namespace chaos::common::async_central;

MetricCollector::MetricCollector(const std::string& _collector_name):
collector_name(_collector_name),
current_slot_index(0),
last_stat_call(0){
    chaos::common::utility::InizializableService::initImplementation(&cacching_slot[0].metric_attribute_cache, NULL, "MetricAttributeChache", __PRETTY_FUNCTION__);
    chaos::common::utility::InizializableService::initImplementation(&cacching_slot[1].metric_attribute_cache, NULL, "MetricAttributeChache", __PRETTY_FUNCTION__);
}

MetricCollector::~MetricCollector() {
    chaos::common::utility::InizializableService::deinitImplementation(&cacching_slot[1].metric_attribute_cache, "MetricAttributeChache", __PRETTY_FUNCTION__);
    chaos::common::utility::InizializableService::deinitImplementation(&cacching_slot[0].metric_attribute_cache, "MetricAttributeChache", __PRETTY_FUNCTION__);
}

bool MetricCollector::addMetric(const std::string& metric_name,
                                chaos::DataType::DataType metric_type,
                                uint32_t metric_max_size) {
    
    cacching_slot[current_slot_index].metric_attribute_cache.addAttribute(metric_name, metric_max_size, metric_type);
    AttributeValue *cached_metric = cacching_slot[current_slot_index].metric_attribute_cache.getValueSettingForIndex(cacching_slot[current_slot_index].metric_attribute_cache.getIndexForName(metric_name));
    if(cached_metric) {
        cacching_slot[current_slot_index].map_attribute_value.insert(make_pair(metric_name, cached_metric));
        return true;
    } else {
        return false;
    }
}

bool MetricCollector::updateMetricValue(const std::string& metric_name,
                                        const void *value_ptr,
                                        uint32_t value_size) {
    boost::shared_lock<boost::shared_mutex> rl(current_slot_index_mutex);
    if(cacching_slot[current_slot_index].map_attribute_value.count(metric_name) == 0) return false;
    return cacching_slot[current_slot_index].map_attribute_value[metric_name]->setValue(value_ptr,
                                                                                        value_size);
}

void MetricCollector::writeTo(chaos::common::data::CDataWrapper& data_wrapper) {
    //switch slot
    boost::unique_lock<boost::shared_mutex> rwl(current_slot_index_mutex);
    ChachingSlot *slot_to_persist =  &cacching_slot[(current_slot_index ^= 0x00000001)];
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
}

void MetricCollector::setStatInterval(uint64_t stat_intervall) {
    AsyncCentralManager::getInstance()->removeTimer(this);
    AsyncCentralManager::getInstance()->addTimer(this, 1000, stat_intervall);
}