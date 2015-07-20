/*
 *	CacheDriver.h
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

#ifndef __CHAOSFramework__CacheDriverSharedMetricIO__
#define __CHAOSFramework__CacheDriverSharedMetricIO__

#include <chaos/common/metric/metric.h>

#include <boost/thread.hpp>

namespace chaos {
    namespace data_service {
        namespace cache_system {
            class CacheDriverSharedMetricIO:
            public chaos::common::metric::MetricCollectorIO {

                //! count the pack that are been set to the cache
                boost::atomic<uint32_t> set_pack_count;
                //! count the total bandwith in set caching operation
                boost::atomic<uint32_t> set_bandwith;
                //! count the pack that are been get to the cache
                boost::atomic<uint32_t> get_pack_count;
                //! ount the total bandwith in set caching operation
                boost::atomic<uint32_t> get_bandwith;
                
                boost::shared_mutex metric_mutex;
            protected:
                void fetchMetricForTimeDiff(uint64_t time_diff);
            public:
                CacheDriverSharedMetricIO(const std::string& client_impl);
                ~CacheDriverSharedMetricIO();
                
                void incrementSetBandWidth(uint32_t increment);
                void incrementGetBandWidth(uint32_t increment);
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__CacheDriverSharedMetricIO__) */
