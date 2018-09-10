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

#include <boost/thread.hpp>

namespace chaos {
    namespace data_service {
        namespace cache_system {
            //! cached drive rofr collecting metric
            /*!
             Virtual driver that permit to colelct metric on cache driver
             */
            class CacheDriverMetricCollector:
            public CacheDriver {
                CacheDriver *wrapped_cache_driver;
                
                boost::mutex mutex_shared_collector;
                static ChaosSharedPtr<CacheDriverSharedMetricIO> shared_collector;
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
                int addServer(std::string server_desc);
                //!inherited method
                int removeServer(std::string server_desc);
                //!inherited method
                int updateConfig();
                //!inherited method
                void init(void *init_data);
                //!inherited method
                void deinit();
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__CacheDriverMetricCollector__) */
