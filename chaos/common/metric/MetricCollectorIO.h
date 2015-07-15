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

#ifndef __CHAOSFramework__MetricCollectorIO__
#define __CHAOSFramework__MetricCollectorIO__

#include <chaos/common/metric/MetricCollector.h>

#include <boost/atomic.hpp>
namespace chaos {
    namespace common {
        namespace metric {
            
            //! class for data io metric collector
            class MetricCollectorIO:
            public MetricCollector {
            protected:
                //! message that are waiting to be sent
                boost::atomic<uint32_t> pack_unsent_count;
                //! messagge that hase beens sent since last metric acquisition
                boost::atomic<uint64_t> pack_count;
                //! toatl bandwith since last metric acquisition
                boost::atomic<uint64_t> bandwith;
                //variable for calculation
                double pack_count_for_ut;
                double bw_for_ut;
                
                void fetchMetricForTimeDiff(uint64_t time_diff);
                
            public:
                MetricCollectorIO(const std::string& _collector_name,
                                  uint64_t update_time_in_sec = 0);
                ~MetricCollectorIO();
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__MetricCollectorIO__) */
