/*
 *	RpcClientMetricCollector.h
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

#ifndef __CHAOSFramework__RpcClientMetricCollector__
#define __CHAOSFramework__RpcClientMetricCollector__

#include <chaos/common/metric/metric.h>
#include <chaos/common/rpc/RpcClient.h>

#include <chaos/common/utility/NamedService.h>
#include <chaos/common/utility/StartableService.h>

#include <boost/atomic.hpp>

namespace chaos {
    namespace common {
        namespace rpc {
            class RpcClientMetricCollector:
            public chaos::common::metric::MetricCollectorIO,
            public RpcClient {
                bool dispose_forwarder_on_exit;
                RpcClient *wrapped_client;
            private:
                void fetchMetricForTimeDiff(uint64_t time_diff);
            public:
                RpcClientMetricCollector(const std::string& forwarder_implementation,
                                         RpcClient *_wrapped_forwarder,
                                         bool _dispose_forwarder_on_exit = true);
                ~RpcClientMetricCollector();
                /*!
                 reimplementation of forwarder abstract method to collect the metric on sent message
                 */
                bool submitMessage(chaos::common::network::NetworkForwardInfo *forward_info, bool on_this_thread=false);
                
                /*
                 init the rpc adapter
                 */
                void init(void *init_data) throw(CException);
                
                /*
                 start the rpc adapter
                 */
                void start() throw(CException);
                
                /*
                 start the rpc adapter
                 */
                void stop() throw(CException);
                
                /*
                 deinit the rpc adapter
                 */
                void deinit() throw(CException);
                
                void setServerHandler(RpcServerHandler *_server_handler);
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__RpcClientMetricCollector__) */
