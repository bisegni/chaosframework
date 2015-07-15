/*
 *	DirectIODispatcherMetricCollector.h
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

#ifndef __CHAOSFramework__DirectIODispatcherMetricCollector__
#define __CHAOSFramework__DirectIODispatcherMetricCollector__

#include <chaos/common/metric/metric.h>
#include <chaos/common/direct_io/DirectIODispatcher.h>

namespace chaos {
    namespace common {
        namespace direct_io {
            
            /*!
             Direct io dispacher subclass that permit to manage
             the metric collection da data that flow through the DirectIO
             */
            class DirectIODispatcherMetricCollector:
            public DirectIODispatcher,
            public chaos::common::metric::MetricCollectorIO {
                int32_t endpoint_alive_count;
            protected:
                void fetchMetricForTimeDiff(uint64_t time_diff);
            public:
                DirectIODispatcherMetricCollector(const std::string& direct_io_server_impl);
                ~DirectIODispatcherMetricCollector();
                
                // Start the implementation
                void start() throw(chaos::CException);
                
                // Stop the implementation
                void stop() throw(chaos::CException);
                
                //! Allocate a new endpoint
                DirectIOServerEndpoint *getNewEndpoint();
                
                //! Relase the endpoint
                void releaseEndpoint(DirectIOServerEndpoint *);
                
                // Event for a new data received
                int priorityDataReceived(DirectIODataPack *data_pack, DirectIOSynchronousAnswerPtr synchronous_answer);
                
                // Event for a new data received
                int serviceDataReceived(DirectIODataPack *data_pack, DirectIOSynchronousAnswerPtr synchronous_answer);
                
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__DirectIODispatcherMetricCollector__) */
