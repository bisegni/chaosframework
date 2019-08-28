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

#ifndef __CHAOSFramework__DirectIOClientConnectionMetricCollector__
#define __CHAOSFramework__DirectIOClientConnectionMetricCollector__

#include <chaos/common/metric/metric.h>
#include <chaos/common/direct_io/DirectIOClientConnection.h>

#include <boost/shared_ptr.hpp>

namespace chaos {
    namespace common {
        namespace direct_io {
            class DirectIOClient;
            /*!
             Direct io dispacher subclass that permit to manage
             the metric collection da data that flow through the DirectIO
             */
            class DirectIOClientConnectionMetricCollector:
            public DirectIOClientConnection,
            public chaos::common::metric::MetricCollectorIO {
                friend class DirectIOClient;
                DirectIOClientConnection *wrapped_connection;
                chaos::common::metric::CounterUniquePtr counter_dataseet_sent;
                chaos::common::metric::CounterUniquePtr counter_data_sent;
                chaos::common::metric::GaugeUniquePtr   gauge_bandwidth_uptr;
                double current_bandwidth;
            protected:
                void fetchMetricForTimeDiff(uint64_t time_diff);
            public:
                DirectIOClientConnectionMetricCollector(const std::string& _server_description,
                                                        uint16_t _endpoint,
                                                        DirectIOClientConnection *_wrapped_connection);
                
                ~DirectIOClientConnectionMetricCollector();
                
                //! inherited method
                int sendPriorityData(chaos::common::direct_io::DirectIODataPackSPtr data_pack);
                int sendPriorityData(chaos::common::direct_io::DirectIODataPackSPtr data_pack,
                                     chaos::common::direct_io::DirectIODataPackSPtr& asynchronous_answer);
                
                
                //! inherited method
                int sendServiceData(chaos::common::direct_io::DirectIODataPackSPtr data_pack);
                int sendServiceData(chaos::common::direct_io::DirectIODataPackSPtr data_pack,
                                    chaos::common::direct_io::DirectIODataPackSPtr& asynchronous_answer);
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__DirectIOClientConnectionMetricCollector__) */
