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
                chaos::common::data::CDWUniquePtr dispatchCommand(chaos::common::data::CDWUniquePtr action_pack);
                
                // execute an action in synchronous mode
                chaos::common::data::CDWUniquePtr executeCommandSync(chaos::common::data::CDWUniquePtr action_pack);
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__RpcServerMetricCollector__) */
