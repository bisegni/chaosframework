/*
 *	StorageDriverMetricIO.h
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

#ifndef __CHAOSFramework__StorageDriverMetricIO__
#define __CHAOSFramework__StorageDriverMetricIO__

#include <chaos/common/metric/metric.h>

#include <boost/atomic.hpp>
#include <boost/thread.hpp>

namespace chaos {
    namespace data_service {
        namespace vfs {
            namespace storage_system {
                
                class StorageDriverMetricIO:
                public chaos::common::metric::MetricCollectorIO {
                    boost::atomic<uint32_t> wr_count;
                    boost::atomic<uint32_t> wr_bandwith;
                    boost::atomic<uint32_t> rd_count;
                    boost::atomic<uint32_t> rd_bandwith;
                    boost::atomic<uint32_t> flush_count;
                    boost::shared_mutex metric_mutex;
                protected:
                    void fetchMetricForTimeDiff(uint64_t time_diff);
                public:
                    StorageDriverMetricIO(const std::string& client_impl);
                    ~StorageDriverMetricIO();
                    
                    void updateWriteBandwith(uint32_t badnwith);
                    void updateReadBandwith(uint32_t badnwith);
                    void incrementFlushCall();
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__StorageDriverMetricIO__) */
