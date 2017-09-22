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

#ifndef __CHAOSFramework__CacheDriverSharedMetricIO__
#define __CHAOSFramework__CacheDriverSharedMetricIO__

#include <chaos/common/metric/metric.h>

#include <boost/atomic.hpp>
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
