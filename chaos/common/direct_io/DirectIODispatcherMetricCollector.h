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
                DirectIODispatcherMetricCollector();
                ~DirectIODispatcherMetricCollector();
                
                // Start the implementation
                void start();
                
                // Stop the implementation
                void stop();
                
                //! Allocate a new endpoint
                DirectIOServerEndpoint *getNewEndpoint();
                
                //! Relase the endpoint
                void releaseEndpoint(DirectIOServerEndpoint *);
                
                // Event for a new data received
                int priorityDataReceived(chaos::common::direct_io::DirectIODataPackSPtr data_pack,
                                         chaos::common::direct_io::DirectIODataPackSPtr& synchronous_answer);
                
                // Event for a new data received
                int serviceDataReceived(chaos::common::direct_io::DirectIODataPackSPtr data_pack,
                                        chaos::common::direct_io::DirectIODataPackSPtr& synchronous_answer);
                
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__DirectIODispatcherMetricCollector__) */
