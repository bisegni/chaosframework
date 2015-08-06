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

#ifndef __CHAOSFramework__DirectIOClientConnectionMetricCollector__
#define __CHAOSFramework__DirectIOClientConnectionMetricCollector__

#include <chaos/common/metric/metric.h>
#include <chaos/common/direct_io/DirectIOClientConnection.h>
#include <chaos/common/direct_io/DirectIOClientConnectionSharedMetricIO.h>

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
            public DirectIOClientConnection {
                friend class DirectIOClient;
                boost::shared_ptr<DirectIOClientConnectionSharedMetricIO> shared_collector;
                DirectIOClientConnection *wrapped_connection;
            protected:
                void fetchMetricForTimeDiff(uint64_t time_diff);
            public:
                DirectIOClientConnectionMetricCollector(const std::string& _server_description,
                                                        uint16_t _endpoint,
                                                        boost::shared_ptr<DirectIOClientConnectionSharedMetricIO> _shared_collector,
                                                        DirectIOClientConnection *_wrapped_connection);
                
                ~DirectIOClientConnectionMetricCollector();
                
                //! inherited method
                int64_t sendPriorityData(DirectIODataPack *data_pack,
                                         DirectIOClientDeallocationHandler *header_deallocation_handler,
                                         DirectIOClientDeallocationHandler *data_deallocation_handler,
                                         DirectIOSynchronousAnswer **asynchronous_answer = NULL);
                
                
                //! inherited method
                int64_t sendServiceData(DirectIODataPack *data_pack,
                                        DirectIOClientDeallocationHandler *header_deallocation_handler,
                                        DirectIOClientDeallocationHandler *data_deallocation_handler,
                                        DirectIOSynchronousAnswer **asynchronous_answer = NULL);
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__DirectIOClientConnectionMetricCollector__) */
