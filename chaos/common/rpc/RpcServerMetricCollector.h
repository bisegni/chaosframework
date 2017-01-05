/*
 *	RpcServerMetricCollector.h
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

#ifndef __CHAOSFramework__RpcServerMetricCollector__
#define __CHAOSFramework__RpcServerMetricCollector__

#include <chaos/common/metric/metric.h>
#include <chaos/common/rpc/RpcServer.h>
#include <chaos/common/rpc/RpcServerHandler.h>


#include <boost/atomic.hpp>

namespace chaos {
    namespace common {
        namespace rpc {
            
            class RpcServerMetricCollector:
            public chaos::common::metric::MetricCollectorIO,
            public RpcServer,
            public RpcServerHandler {
                bool dispose_forwarder_on_exit;
                RpcServer *wrapper_server;
                RpcServerHandler *wrapperd_server_handler;
 
                void fetchMetricForTimeDiff(uint64_t time_diff);
            public:
                RpcServerMetricCollector(const std::string& forwarder_implementation,
                                         RpcServer *_wrapper_server,
                                         bool _dispose_forwarder_on_exit = true);
                
                ~RpcServerMetricCollector();
                
                /*
                 init the rpc adapter
                 */
                virtual void init(void*) throw(CException);
                
                /*
                 start the rpc adapter
                 */
                virtual void start() throw(CException);
                
                /*
                 start the rpc adapter
                 */
                virtual void stop() throw(CException);
                
                /*
                 deinit the rpc adapter
                 */
                virtual void deinit() throw(CException);
                
                /*!
                 Return the published port
                 */
                int getPublishedPort();
                
                /*
                 set the command dispatcher associated to the instance of rpc adapter
                 */
                void setCommandDispatcher(RpcServerHandler *_wrapperd_server_handler);
                
                //-------server handler implementation method------------
                // method called when the rpc server receive a new data
                chaos_data::CDataWrapper* dispatchCommand(chaos_data::CDataWrapper * action_pack)  throw(CException);
                
                // execute an action in synchronous mode
                chaos_data::CDataWrapper* executeCommandSync(chaos_data::CDataWrapper * action_pack);
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__RpcServerMetricCollector__) */
