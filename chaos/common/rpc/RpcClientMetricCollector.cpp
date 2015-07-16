/*
 *	RpcClientMetricCollector.cpp
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

#include <chaos/common/configuration/GlobalConfiguration.h>
#include <chaos/common/rpc/RpcClientMetricCollector.h>
#include <boost/format.hpp>

using namespace chaos::common::rpc;
#define RPCMFC_INFO INFO_LOG(RpcClientMetricCollector)
#define RPCMFC_LDBG DBG_LOG(RpcClientMetricCollector)
#define RPCMFC_LERR ERR_LOG(RpcClientMetricCollector)

RpcClientMetricCollector::RpcClientMetricCollector(const std::string& forwarder_implementation,
                                                   RpcClient *_wrapped_client,
                                                   bool _dispose_forwarder_on_exit):
MetricCollectorIO(forwarder_implementation,
                GlobalConfiguration::getInstance()->getConfiguration()->getUInt64Value(InitOption::OPT_RPC_LOG_METRIC_UPDATE_INTERVAL)),
RpcClient(forwarder_implementation),
wrapped_client(_wrapped_client),
dispose_forwarder_on_exit(_dispose_forwarder_on_exit) {
    RPCMFC_LDBG << "Allocate collector";
    //set the time interval to one second of default
    addBackend(metric::MetricBackendPointer(new metric::ConsoleMetricBackend(getName())));
}

RpcClientMetricCollector::~RpcClientMetricCollector() {
    RPCMFC_LDBG << "Deallocate collector";
    if(dispose_forwarder_on_exit)  CHK_AND_DELETE_OBJ_POINTER(wrapped_client)
}

/*
 init the rpc adapter
 */
void RpcClientMetricCollector::init(void *init_data) throw(CException) {
    CHAOS_ASSERT(wrapped_client)
    utility::StartableService::initImplementation(wrapped_client, init_data, wrapped_client->getName(), __PRETTY_FUNCTION__);
}

/*
 start the rpc adapter
 */
void RpcClientMetricCollector::start() throw(CException) {
    CHAOS_ASSERT(wrapped_client)
    startLogging();
    utility::StartableService::startImplementation(wrapped_client, wrapped_client->getName(), __PRETTY_FUNCTION__);
}

/*
 start the rpc adapter
 */
void RpcClientMetricCollector::stop() throw(CException) {
    CHAOS_ASSERT(wrapped_client)
    stopLogging();
    utility::StartableService::stopImplementation(wrapped_client, wrapped_client->getName(), __PRETTY_FUNCTION__);
}

/*
 deinit the rpc adapter
 */
void RpcClientMetricCollector::deinit() throw(CException) {
    CHAOS_ASSERT(wrapped_client)
    utility::StartableService::deinitImplementation(wrapped_client, wrapped_client->getName(), __PRETTY_FUNCTION__);
}

void RpcClientMetricCollector::setServerHandler(RpcServerHandler *_server_handler) {
    CHAOS_ASSERT(wrapped_client)
    wrapped_client->setServerHandler(_server_handler);
}

bool RpcClientMetricCollector::submitMessage(chaos::common::network::NetworkForwardInfo *forward_info, bool on_this_thread) {
    CHAOS_ASSERT(wrapped_client)
    int size = 0;
    bool result = true;
    
    result = wrapped_client->submitMessage(forward_info, on_this_thread);
    
    //inrement packec count
    pack_count++;
    
    //increment packet size
    forward_info->message->getBSONRawData(size);
    bandwith+=size;
    
    return result;
}

void RpcClientMetricCollector::fetchMetricForTimeDiff(uint64_t time_diff) {
    pack_unsent_count = (uint32_t)wrapped_client->getMessageQueueSize();
    MetricCollectorIO::fetchMetricForTimeDiff(time_diff);
}