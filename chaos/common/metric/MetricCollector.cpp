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

#include <chaos/common/metric/MetricCollector.h>
#include <chaos/common/metric/ConsoleMetricBackend.h>
#include <chaos/common/metric/CSVFileMetricBackend.h>
#include <chaos/common/utility/TimingUtil.h>
#include <chaos/common/configuration/GlobalConfiguration.h>
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
    bool on_console = GlobalConfiguration::getInstance()->getConfiguration()->hasKey(InitOption::OPT_LOG_METRIC_ON_CONSOLE) &&
    GlobalConfiguration::getInstance()->getConfiguration()->getVariantValue(InitOption::OPT_LOG_METRIC_ON_CONSOLE).asBool();
    bool on_file = GlobalConfiguration::getInstance()->getConfiguration()->hasKey(InitOption::OPT_LOG_METRIC_ON_FILE) &&
    GlobalConfiguration::getInstance()->getConfiguration()->getVariantValue(InitOption::OPT_LOG_METRIC_ON_FILE).asBool();
    if(on_console) {
        //set the metric manager to print metrics on console
        addBackend(metric::MetricBackendPointer(new metric::ConsoleMetricBackend(collector_name)));
    }
    
    if(on_file) {
        const std::string file_path = GlobalConfiguration::getInstance()->getConfiguration()->getStringValue(InitOption::OPT_LOG_METRIC_ON_FILE_PATH);
        //set the time interval to one second of default
        addBackend(metric::MetricBackendPointer(new metric::CVSFileMetricBackend(collector_name,
                                                                                 file_path)));
    }
    if(!on_console &&
       !on_file) {
           MC_INFO<< "% No metric output selected for collector, redirecting to default. To redirect output use options:'"<<InitOption::OPT_LOG_METRIC_ON_FILE<<"' and '"<<InitOption::OPT_LOG_METRIC_ON_FILE_PATH<<"'" ;
        addBackend(metric::MetricBackendPointer(new metric::ConsoleMetricBackend(collector_name)));
        // per una cavolata non parte l'MDS
        //throw CException(-1, "No metric output selected for collector", __PRETTY_FUNCTION__);
    }
}

MetricCollector::~MetricCollector() {
    stopLogging();
    caching_slot[1].metric_attribute_cache.reset();
    caching_slot[0].metric_attribute_cache.reset();
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
        cs.map_name_index.insert(make_pair(metric_name, cs.map_name_index.size()));
        cs.vector_attribute_value.push_back(cached_metric);
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
    if(caching_slot[current_slot_index].map_name_index.count(metric_name) == 0) return false;
    return caching_slot[current_slot_index].vector_attribute_value[caching_slot[current_slot_index].map_name_index[metric_name]]->setValue(value_ptr,
                                                                                                                                           value_size);
}

void MetricCollector::writeTo(chaos::common::data::CDataWrapper& data_wrapper) {
    //switch slot
    boost::unique_lock<boost::shared_mutex> rwl(current_slot_index_mutex);
    ChachingSlot *slot_to_persist =  &caching_slot[(current_slot_index ^= 0x00000001)];
    rwl.unlock();
    
    //persiste previous slot
    for(VectorMetricIterator it = slot_to_persist->vector_attribute_value.begin();
        it != slot_to_persist->vector_attribute_value.end();
        it++) {
        (*it)->writeToCDataWrapper(data_wrapper);
    }
}

void MetricCollector::timeout() {
    uint64_t now = chaos::common::utility::TimingUtil::getTimeStamp();
    if(last_stat_call) {
        fetchMetricForTimeDiff(now - last_stat_call);
    } else {
        fetchMetricForTimeDiff(0);
    }
    last_stat_call = chaos::common::utility::TimingUtil::getTimeStamp();
    //switch slot
    boost::unique_lock<boost::shared_mutex> rwl(current_slot_index_mutex);
    ChachingSlot *slot_to_persist =  &caching_slot[(current_slot_index ^= 0x00000001) ^ 0x00000001];
    rwl.unlock();
    
    //write metric to backend
    CHAOS_SCAN_VECTOR_ITERATOR(VectorMetricBackendIterator, vector_metric_backend, (*it)->prepare(now);)
    for(VectorMetricIterator it_metric = slot_to_persist->vector_attribute_value.begin();
        it_metric != slot_to_persist->vector_attribute_value.end();
        it_metric++) {
        CHAOS_SCAN_VECTOR_ITERATOR(VectorMetricBackendIterator,
                                   vector_metric_backend,
                                   (*it)->preMetric();
                                   (*it)->addMetric((*it_metric)->name,
                                                    (*it_metric)->toString(2));
                                   (*it)->postMetric();)
    }
    CHAOS_SCAN_VECTOR_ITERATOR(VectorMetricBackendIterator, vector_metric_backend, (*it)->flush();)
}

void MetricCollector::setStatIntervalInSeconds(uint64_t stat_intervall_in_seconds) {
    stat_intervall = stat_intervall_in_seconds*1000;
}

void MetricCollector::startLogging() {
    try{
        last_stat_call = chaos::common::utility::TimingUtil::getTimeStamp();
        AsyncCentralManager::getInstance()->addTimer(this, 1000, stat_intervall);
    }catch(...) {
        throw CException(-1, "Error adding timer", __PRETTY_FUNCTION__);
    }
}

void MetricCollector::stopLogging() {
    AsyncCentralManager::getInstance()->removeTimer(this);
}
