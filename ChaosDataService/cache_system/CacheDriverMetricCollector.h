/*
 *	CacheDriverMetricCollector.h
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
                static boost::shared_ptr<CacheDriverSharedMetricIO> shared_collector;
            public:
                //! base constructo rwith the wrapped dariver
                CacheDriverMetricCollector(CacheDriver *_wrapped_cache_driver);
                ~CacheDriverMetricCollector();
                //!inherited method
                int putData(void *element_key,
                                    uint8_t element_key_len,
                                    void *value,
                                    uint32_t value_len);
                //!inherited method
                int getData(void *element_key,
                                    uint8_t element_key_len,
                                    void **value,
                                    uint32_t& value_len);
                //!inherited method
                int addServer(std::string server_desc);
                //!inherited method
                int removeServer(std::string server_desc);
                //!inherited method
                int updateConfig();
                //!inherited method
                void init(void *init_data) throw (chaos::CException);
                //!inherited method
                void deinit() throw (chaos::CException);
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__CacheDriverMetricCollector__) */
