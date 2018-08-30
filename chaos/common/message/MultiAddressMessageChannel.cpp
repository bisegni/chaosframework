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

#include <chaos/common/utility/TimingUtil.h>
#include <chaos/common/message/MultiAddressMessageChannel.h>

#include <boost/format.hpp>
using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::message;
using namespace chaos::common::network;
using namespace chaos::common::async_central;

#define MAMC_INFO INFO_LOG(MultiAddressMessageChannel)
#define MAMC_DBG DBG_LOG(MultiAddressMessageChannel)
#define MAMC_ERR ERR_LOG(MultiAddressMessageChannel)

#define RETRY_DELAY 30000
//! default constructor
MultiAddressMessageChannel::MultiAddressMessageChannel(NetworkBroker *message_broker,
                                                       MessageRequestDomainSHRDPtr _new_message_request_domain):
MessageChannel(message_broker,
               _new_message_request_domain),
service_feeder("MultiAddressMessageChannel",
               this,
               this),
last_retry(0){}

//!Base constructor
MultiAddressMessageChannel::MultiAddressMessageChannel(NetworkBroker *message_broker,
                                                       CNetworkAddress& node_address,
                                                       MessageRequestDomainSHRDPtr _new_message_request_domain):
MessageChannel(message_broker,
               _new_message_request_domain),
service_feeder("MultiAddressMessageChannel",
               this,
               this),
last_retry(0){
    addNode(node_address);
}

//!Base constructor
MultiAddressMessageChannel::MultiAddressMessageChannel(NetworkBroker *message_broker,
                                                       const std::vector<CNetworkAddress>& node_address,
                                                       MessageRequestDomainSHRDPtr _new_message_request_domain):
MessageChannel(message_broker,
               _new_message_request_domain),
service_feeder("MultiAddressMessageChannel",
               this,
               this),
last_retry(0){
    for(std::vector<CNetworkAddress>::const_iterator it = node_address.begin();
        it != node_address.end();
        it++) {
        addNode(*it);
    }
}

//
MultiAddressMessageChannel::~MultiAddressMessageChannel() {
    service_feeder.clear();
}

void MultiAddressMessageChannel::init() throw(CException) {
    MessageChannel::init();
}

void MultiAddressMessageChannel::deinit() throw(CException) {
    AsyncCentralManager::getInstance()->removeTimer(this);
    MessageChannel::deinit();
}
void MultiAddressMessageChannel::setURLAsOffline(const std::string& offline_url) {
    service_feeder.setURLAsOffline(offline_url);
    AsyncCentralManager::getInstance()->addTimer(this,
                                                 1000,
                                                 1000);
}
//
void MultiAddressMessageChannel::addNode(const CNetworkAddress& node_address) {
    service_feeder.addURL(chaos::common::network::URL(node_address.ip_port));
}

//
void MultiAddressMessageChannel::removeNode(const CNetworkAddress& node_address) {
    service_feeder.removeURL(node_address.ip_port);
}

//! remove all configured node
void MultiAddressMessageChannel::removeAllNode() {
    service_feeder.clear(true);
}

//
void  MultiAddressMessageChannel::disposeService(void *service_ptr) {
    MMCFeederService *service = static_cast<MMCFeederService*>(service_ptr);
    if(service) delete(service);
}

//
void* MultiAddressMessageChannel::serviceForURL(const URL& url,
                                                uint32_t service_index) {
    MMCFeederService *service = new MMCFeederService(url.getURL());
    return static_cast<void*>(service);
}

bool MultiAddressMessageChannel::serviceOnlineCheck(void *service_ptr) {
    bool result = false;
    int retry = 3;
    MMCFeederService *service = static_cast<MMCFeederService*>(service_ptr);
    ChaosUniquePtr<MessageRequestFuture> request = MessageChannel::echoTest(service->ip_port,
                                                                            CDWUniquePtr());
    while(--retry>0) {
        if(request->wait(2000)) {
            retry = 0;
            result = (request->getError() == 0);
        }
    }
    return result;
}

void MultiAddressMessageChannel::timeout() {
    service_feeder.checkForAliveService();
    if(service_feeder.getOfflineSize() == 0) {
        AsyncCentralManager::getInstance()->removeTimer(this);
    }
}

//! get the rpc published host and port
void MultiAddressMessageChannel::getRpcPublishedHostAndPort(std::string& rpc_published_host_port) {
    MessageChannel::getRpcPublishedHostAndPort(rpc_published_host_port);
}

//! send a message
void MultiAddressMessageChannel::sendMessage(const std::string& action_domain,
                                             const std::string& action_name,
                                             CDWUniquePtr message_pack,
                                             bool on_this_thread) {
    MMCFeederService *service =  static_cast<MMCFeederService*>(service_feeder.getService());
    if(service ) {
        MessageChannel::sendMessage(service->ip_port,
                                    action_domain,
                                    action_name,
                                    MOVE(message_pack));
        DEBUG_CODE(MAMC_DBG << "Sent message to:" << service->ip_port;)
    }
    
}

//!send an rpc request to a remote node
ChaosUniquePtr<MessageRequestFuture> MultiAddressMessageChannel::_sendRequestWithFuture(const std::string& action_domain,
                                                                                        const std::string& action_name,
                                                                                        CDWUniquePtr request_pack,
                                                                                        std::string& used_remote_address) {
    ChaosUniquePtr<MessageRequestFuture> result;
    MMCFeederService *service =  static_cast<MMCFeederService*>(service_feeder.getService());
    if(service) {
        result = MessageChannel::sendRequestWithFuture((used_remote_address = service->ip_port),
                                                       action_domain,
                                                       action_name,
                                                       MOVE(request_pack));
        DEBUG_CODE(MAMC_DBG << "Sent request to:" << used_remote_address;)

    } else {
        used_remote_address.clear();
    }
    return result;
}

//!send an rpc request to a remote node
ChaosUniquePtr<MultiAddressMessageRequestFuture> MultiAddressMessageChannel::sendRequestWithFuture(const std::string& action_domain,
                                                                                                   const std::string& action_name,
                                                                                                   CDWUniquePtr request_pack,
                                                                                                   int32_t request_timeout) {
    return ChaosUniquePtr<MultiAddressMessageRequestFuture>(new MultiAddressMessageRequestFuture(this,
                                                                                                 action_domain,
                                                                                                 action_name,
                                                                                                 MOVE(request_pack),
                                                                                                 request_timeout));
}

