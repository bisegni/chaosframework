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

#ifndef __CHAOSFramework__CacheDriverMetricCollector__
#define __CHAOSFramework__CacheDriverMetricCollector__

#include "CacheDriverSharedMetricIO.h"
#include "CacheDriver.h"

#include <chaos/common/metric/metric.h>

#include <boost/thread.hpp>

namespace chaos {
    namespace metadata_service {
        namespace cache_system {
            //! cached drive rofr collecting metric
            /*!
             Virtual driver that permit to colelct metric on cache driver
             */
            class CacheDriverMetricCollector:
            public CacheDriver {
                CacheDriver *wrapped_cache_driver;
                //! count the pack that are been set to the cache
                chaos::common::metric::CounterUniquePtr set_pack_count_uptr;
                //! count the total bandwith in set caching operation
                chaos::common::metric::CounterUniquePtr set_bandwith_uptr;
                //! count the pack that are been get to the cache
                chaos::common::metric::CounterUniquePtr get_pack_count_uptr;
                //! ount the total bandwith in set caching operation
                chaos::common::metric::CounterUniquePtr get_bandwith_uptr;
            public:
                //!base constructo rwith the wrapped dariver
                CacheDriverMetricCollector(CacheDriver *_wrapped_cache_driver);
                ~CacheDriverMetricCollector();
                //!inherited method
                int putData(const std::string& key,
                            CacheData data);
                //!inherited method
                int getData(const std::string& key,
                            CacheData& data);
                //!inherited method
                int getData(const ChaosStringVector&    keys,
                            MultiCacheData&             multi_data);
                //!inherited method
                int addServer(const std::string& server_desc);
                //!inherited method
                int removeServer(const std::string& server_desc);
                //!inherited method
                void init(void *init_data);
                //!inherited method
                void deinit();
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__CacheDriverMetricCollector__) */
