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

#include <stdint.h>
#include <string>

#include <boost/thread.hpp>

namespace chaos {
    namespace common {
        namespace metric {
            
            CHAOS_DEFINE_MAP_FOR_TYPE(std::string, chaos::common::data::cache::AttributeValue*, MapMetric )
            
            struct ChachingSlot {
                MapMetric map_attribute_value;
                chaos::common::data::cache::AttributeCache metric_attribute_cache;
            };
            
            //! Class that permit to collect a set of metric
            class MetricCollector:
            public chaos::common::async_central::TimerHandler {
                //is a name for collector
                std::string collector_name;
                
                ChachingSlot cacching_slot[2];
                
                boost::shared_mutex current_slot_index_mutex;
                int  current_slot_index;
                uint64_t last_stat_call;
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
                
                //!set the stat interval in milliseoconds
                /*!
                 determinate the delay between a fetchMetricForTimeDiff call and the next
                 */
                void setStatInterval(uint64_t stat_intervall);
            public:
                MetricCollector(const std::string& _collector_name);
                ~MetricCollector();
                bool addMetric(const std::string& metric_name,
                               chaos::DataType::DataType metric_type,
                               uint32_t metric_max_size = 32);

                bool updateMetricValue(const std::string& metric_name,
                                       const void *value_ptr,
                                       uint32_t value_size);
                
                void writeTo(chaos::common::data::CDataWrapper& data_wrapper);
            };
            
        }
    }
}

#endif /* defined(__CHAOSFramework__MetricCollector__) */
