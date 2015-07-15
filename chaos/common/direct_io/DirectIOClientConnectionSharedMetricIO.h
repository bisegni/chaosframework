/*
 *	DirectIOClientConnectionMetricCollector.h
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

#ifndef __CHAOSFramework__DirectIOClientConnectionSharedMetricIO__
#define __CHAOSFramework__DirectIOClientConnectionSharedMetricIO__

#include <chaos/common/metric/metric.h>
namespace chaos {
    namespace common {
        namespace direct_io {
            class DirectIOClientConnectionSharedMetricIO:
            public chaos::common::metric::MetricCollectorIO {
            protected:
                void fetchMetricForTimeDiff(uint64_t time_diff);
            public:
                DirectIOClientConnectionSharedMetricIO(const std::string& client_impl,
                                                       const std::string& server_endpoint);
                ~DirectIOClientConnectionSharedMetricIO();
                
                void incrementPackCount();
                void incrementBandWidth(uint64_t increment);
            };
        }
    }
}
#endif /* defined(__CHAOSFramework__DirectIOClientConnectionSharedMetricIO__) */
