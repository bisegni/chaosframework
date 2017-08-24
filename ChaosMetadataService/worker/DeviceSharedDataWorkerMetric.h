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
