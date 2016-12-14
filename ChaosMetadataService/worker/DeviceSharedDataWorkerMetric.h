/*
 *	DeviceSharedDataWorkerMetric.h
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

#ifndef __CHAOSFramework__DeviceSharedDataWorkerMetric__
#define __CHAOSFramework__DeviceSharedDataWorkerMetric__

#include <chaos/common/metric/metric.h>

#include <boost/thread.hpp>
#include <boost/atomic.hpp>

namespace chaos{
    namespace data_service {
        namespace worker {
            
            class DeviceSharedDataWorkerMetric :
            public chaos::common::metric::MetricCollector {
                
                //! count the pack that are been set to the cache
                boost::atomic<uint32_t> input_data_pack;
                boost::atomic<uint64_t> input_badnwith;
                boost::atomic<uint32_t> output_data_pack;
                boost::atomic<uint64_t> output_badnwith;
                boost::atomic<uint32_t> queued_data_pack;
                boost::atomic<uint64_t> queued_size;
                
                boost::shared_mutex metric_mutex;
            protected:
                void fetchMetricForTimeDiff(uint64_t time_diff);
            public:
                DeviceSharedDataWorkerMetric(const std::string& client_impl,
                                             uint64_t update_time_in_sec);
                ~DeviceSharedDataWorkerMetric();
                
                void incrementInputBandwith(uint32_t pack_size);
                void incrementOutputBandwith(uint32_t pack_size);
                void incrementQueueSize(uint32_t pack_size);
                void decrementQueueSize(uint32_t pack_size);
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__DeviceSharedDataWorkerMetric__) */
