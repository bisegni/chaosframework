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
    namespace metadata_service {
        namespace cache_system {
            class CacheDriverSharedMetricIO:
            public chaos::common::metric::MetricCollectorIO {
                
                //! count the pack that are been set to the cache
                chaos::common::metric::CounterUniquePtr set_pack_count_uptr;
                //! count the total bandwith in set caching operation
                chaos::common::metric::CounterUniquePtr set_bandwith_uptr;
                //! count the pack that are been get to the cache
                chaos::common::metric::CounterUniquePtr get_pack_count_uptr;
                //! ount the total bandwith in set caching operation
                chaos::common::metric::CounterUniquePtr get_bandwith_uptr;
                
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
