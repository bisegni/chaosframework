/**
 *	RpcServerMetricCollector.cpp
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

#include <chaos/common/rpc/RpcServerMetricCollector.h>
#include <chaos/common/configuration/GlobalConfiguration.h>

using namespace chaos;
using namespace chaos::common;
using namespace chaos::common::rpc;
using namespace chaos::common::metric;

#define RPCSMC_INFO INFO_LOG(RpcServerMetricCollector)
#define RPCSMC_LDBG DBG_LOG(RpcClientMetricCollector)
#define RPCSMC_LERR ERR_LOG(RpcClientMetricCollector)

RpcServerMetricCollector::RpcServerMetricCollector(const std::string& forwarder_implementation,
                                                   RpcServer *_wrapper_server,
                                                   bool _dispose_forwarder_on_exit):
MetricCollectorIO(forwarder_implementation,
                  GlobalConfiguration::getInstance()->getConfiguration()->getUInt64Value(InitOption::OPT_RPC_LOG_METRIC_UPDATE_INTERVAL)),
RpcServer(forwarder_implementation),
wrapper_server(_wrapper_server),
wrapperd_server_handler(NULL),
dispose_forwarder_on_exit(_dispose_forwarder_on_exit){
    CHAOS_ASSERT(wrapper_server)
    RPCSMC_LDBG << "Allocate collector";
    wrapper_server->setCommandDispatcher(this);
}

RpcServerMetricCollector::~RpcServerMetricCollector() {
    RPCSMC_LDBG << "Deallocate collector";
    if(dispose_forwarder_on_exit) {
        DELETE_OBJ_POINTER(wrapper_server)
    }
}

/*
 init the rpc adapter
 */
void RpcServerMetricCollector::init(void *init_data) throw(CException) {
    CHAOS_ASSERT(wrapper_server)
    utility::StartableService::initImplementation(wrapper_server, init_data, wrapper_server->getName(), __PRETTY_FUNCTION__);
}

/*
 start the rpc adapter
 */
void RpcServerMetricCollector::start() throw(CException) {
    CHAOS_ASSERT(wrapper_server)
    startLogging();
    utility::StartableService::startImplementation(wrapper_server, wrapper_server->getName(), __PRETTY_FUNCTION__);
}

/*
 start the rpc adapter
 */
void RpcServerMetricCollector::stop() throw(CException) {
    CHAOS_ASSERT(wrapper_server)
    stopLogging();
    utility::StartableService::stopImplementation(wrapper_server, wrapper_server->getName(), __PRETTY_FUNCTION__);
}

/*
 deinit the rpc adapter
 */
void RpcServerMetricCollector::deinit() throw(CException) {
    CHAOS_ASSERT(wrapper_server)
    utility::StartableService::deinitImplementation(wrapper_server, wrapper_server->getName(), __PRETTY_FUNCTION__);
}


int RpcServerMetricCollector::getPublishedPort() {
    CHAOS_ASSERT(wrapper_server)
    return wrapper_server->getPublishedPort();
}

void RpcServerMetricCollector::setCommandDispatcher(RpcServerHandler *_wrapperd_server_handler) {
    CHAOS_ASSERT(wrapper_server)
    wrapperd_server_handler = _wrapperd_server_handler;
}

// method called when the rpc server receive a new data
chaos_data::CDataWrapper* RpcServerMetricCollector::dispatchCommand(chaos_data::CDataWrapper * action_pack)  throw(CException) {
    CHAOS_ASSERT(wrapperd_server_handler)
    int size = 0;
    chaos_data::CDataWrapper *result = NULL;
    //inrement packec count
    pack_count++;
    result = wrapperd_server_handler->dispatchCommand(action_pack);
    //increment packet size
    action_pack->getBSONRawData(size);
    bandwith+=size;
    return result;
}

// execute an action in synchronous mode
chaos_data::CDataWrapper* RpcServerMetricCollector::executeCommandSync(chaos_data::CDataWrapper * action_pack) {
    CHAOS_ASSERT(wrapperd_server_handler)
    int size = 0;
    chaos_data::CDataWrapper *result = NULL;
    //inrement packec count
    pack_count++;
    result = wrapperd_server_handler->dispatchCommand(action_pack);
    //increment packet size
    action_pack->getBSONRawData(size);
    bandwith+=size;
    return result;
}

void RpcServerMetricCollector::fetchMetricForTimeDiff(uint64_t time_diff) {
    MetricCollectorIO::fetchMetricForTimeDiff(time_diff);
}
