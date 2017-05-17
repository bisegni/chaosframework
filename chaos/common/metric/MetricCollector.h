/*
 *	MetricCollector.h
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

#ifndef __CHAOSFramework__MetricCollector__
#define __CHAOSFramework__MetricCollector__

#include <chaos/common/chaos_types.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/data/cache/AttributeCache.h>
#include <chaos/common/async_central/async_central.h>
#include <chaos/common/metric/AbstractMetricBackend.h>

#include <string>
#include <stdint.h>

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

namespace chaos {
    namespace common {
        namespace metric {
            
            typedef ChaosSharedPtr<AbstractMetricBackend> MetricBackendPointer;
            CHAOS_DEFINE_VECTOR_FOR_TYPE(MetricBackendPointer, VectorMetricBackend)
            
            CHAOS_DEFINE_MAP_FOR_TYPE(std::string, int, MapMetricNameVecIndex)
            CHAOS_DEFINE_VECTOR_FOR_TYPE(chaos::common::data::cache::AttributeValue*, VectorMetric)
            
            struct ChachingSlot {
                MapMetricNameVecIndex map_name_index;
                VectorMetric vector_attribute_value;
                chaos::common::data::cache::AttributeCache metric_attribute_cache;
            };
            
            //! Class that permit to collect a set of metric
            class MetricCollector:
            public chaos::common::async_central::TimerHandler {
                //is a name for collector
                std::string collector_name;

                //! backend used to visualize the metric
                VectorMetricBackend vector_metric_backend;
                
                //! two different slot for optimizing the update of the metric wil old one are snapped to backend
                ChachingSlot caching_slot[2];
                
                boost::shared_mutex current_slot_index_mutex;
                int  current_slot_index;
                uint64_t last_stat_call;
                uint64_t stat_intervall;
                
                bool _addMetric(ChachingSlot& cs,
                                const std::string& metric_name,
                                chaos::DataType::DataType metric_type,
                                uint32_t metric_max_size = 32);
                
                void _prepareBackends();
                void _sendToBackends(const std::string& metric_name,
                                    const std::string& metric_value);
                void _flushBackends();
            private:
                void timeout();
            protected:
                //! need to be implementad by the subclass for collect metric
                /*!
                 this methdo is called at fixed(almost) delay of time, every time si called
                 it bring the difference in milliseconds since the last call. Subclass can usi this value
                 to compute the metric of statistic
                 \param time_diff time passed(in milliseconds) from last call
                 */
                virtual void fetchMetricForTimeDiff(uint64_t time_diff) = 0;
                
            public:
                MetricCollector(const std::string& _collector_name,
                                uint64_t update_time_in_sec = 10);
                ~MetricCollector();
                
                //! add a new backend
                void addBackend(MetricBackendPointer backend);
                
                //add a new metric
                bool addMetric(const std::string& metric_name,
                               chaos::DataType::DataType metric_type,
                               uint32_t metric_max_size = 32);

                //udpate a single metric value
                bool updateMetricValue(const std::string& metric_name,
                                       const void *value_ptr,
                                       uint32_t value_size);
                
                //! write the mertic to a cdata wrapper
                void writeTo(chaos::common::data::CDataWrapper& data_wrapper);
                
                //!set the stat interval in milliseoconds
                /*!
                 determinate the delay between a fetchMetricForTimeDiff call and the next
                 */
                void setStatIntervalInSeconds(uint64_t stat_intervall_in_seconds);
                
                void startLogging();
                
                void stopLogging();
            };
            
        }
    }
}

#endif /* defined(__CHAOSFramework__MetricCollector__) */
