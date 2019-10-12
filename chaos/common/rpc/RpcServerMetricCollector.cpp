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

#include <chaos/common/rpc/RpcServerMetricCollector.h>
#include <chaos/common/configuration/GlobalConfiguration.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::rpc;
using namespace chaos::common::metric;

#define RPCSMC_INFO INFO_LOG(RpcServerMetricCollector)
#define RPCSMC_LDBG DBG_LOG(RpcClientMetricCollector)
#define RPCSMC_LERR ERR_LOG(RpcClientMetricCollector)

RpcServerMetricCollector::RpcServerMetricCollector(const std::string& forwarder_implementation,
                                                   RpcServer *_wrapper_server,
                                                   bool _dispose_forwarder_on_exit):
MetricCollectorIO(),
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
void RpcServerMetricCollector::init(void *init_data) {
    CHAOS_ASSERT(wrapper_server)
    utility::StartableService::initImplementation(wrapper_server, init_data, wrapper_server->getName(), __PRETTY_FUNCTION__);
    packet_async_count_uptr = MetricManager::getInstance()->getNewRxPacketRateMetricFamily({{"driver","rpc_async"}});
    data_async_count_uptr = MetricManager::getInstance()->getNewRxDataRateMetricFamily({{"driver","rpc_async"}});
    packet_sync_count_uptr = MetricManager::getInstance()->getNewRxPacketRateMetricFamily({{"driver","rpc_sync"}});
    data_sync_count_uptr = MetricManager::getInstance()->getNewRxDataRateMetricFamily({{"driver","rpc_sync"}});
}

/*
 start the rpc adapter
 */
void RpcServerMetricCollector::start() {
    CHAOS_ASSERT(wrapper_server)
    startLogging();
    utility::StartableService::startImplementation(wrapper_server, wrapper_server->getName(), __PRETTY_FUNCTION__);
}

/*
 start the rpc adapter
 */
void RpcServerMetricCollector::stop() {
    CHAOS_ASSERT(wrapper_server)
    stopLogging();
    utility::StartableService::stopImplementation(wrapper_server, wrapper_server->getName(), __PRETTY_FUNCTION__);
}

/*
 deinit the rpc adapter
 */
void RpcServerMetricCollector::deinit() {
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
CDWUniquePtr RpcServerMetricCollector::dispatchCommand(CDWUniquePtr action_pack) {
    CHAOS_ASSERT(wrapperd_server_handler)
    CDWUniquePtr result;
    //inrement packec count
    (*packet_sync_count_uptr)++;
    (*data_sync_count_uptr) += action_pack->getBSONRawSize();
    result = wrapperd_server_handler->dispatchCommand(MOVE(action_pack));
    return result;
}

// execute an action in synchronous mode
CDWUniquePtr RpcServerMetricCollector::executeCommandSync(CDWUniquePtr action_pack) {
    CHAOS_ASSERT(wrapperd_server_handler)
    CDWUniquePtr result;
    //inrement packec count
    (*packet_async_count_uptr)++;
    (*data_async_count_uptr) += action_pack->getBSONRawSize();
    return wrapperd_server_handler->dispatchCommand(MOVE(action_pack));
}

void RpcServerMetricCollector::fetchMetricForTimeDiff(uint64_t time_diff) {
}
