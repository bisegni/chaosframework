/*
 *	MultiAddressMessageChannel.cpp
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

#include <chaos/common/utility/TimingUtil.h>
#include <chaos/common/message/MultiAddressMessageChannel.h>

#include <boost/format.hpp>
using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::message;
using namespace chaos::common::network;

#define MAMC_INFO INFO_LOG(MultiAddressMessageChannel)
#define MAMC_DBG DBG_LOG(MultiAddressMessageChannel)
#define MAMC_ERR ERR_LOG(MultiAddressMessageChannel)

#define RETRY_DELAY 30000
    //! default constructor
MultiAddressMessageChannel::MultiAddressMessageChannel(NetworkBroker *message_broker):
MessageChannel(message_broker),
service_feeder("MultiAddressMessageChannel", this),
last_retry(0){

}

    //!Base constructor
MultiAddressMessageChannel::MultiAddressMessageChannel(NetworkBroker *message_broker,
                                                       CNetworkAddress& node_address):
MessageChannel(message_broker),
service_feeder("MultiAddressMessageChannel", this),
last_retry(0){
    addNode(node_address);
}

    //!Base constructor
MultiAddressMessageChannel::MultiAddressMessageChannel(NetworkBroker *message_broker,
                                                       const std::vector<CNetworkAddress>& node_address):
MessageChannel(message_broker),
service_feeder("MultiAddressMessageChannel", this),
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

void MultiAddressMessageChannel::retryOfflineServer(bool force) {
    boost::lock_guard<boost::mutex> l(mutex_server_usage);
    uint64_t now = TimingUtil::getTimeStamp();
    if(force||
       ((now - last_retry) > RETRY_DELAY)) {
        MAMC_INFO << "Retry on all server";
        last_retry = now;
        std::set<uint32_t>::iterator it = set_off_line_servers.begin();
        while(it != set_off_line_servers.end()) {
            service_feeder.setURLOnline(*it);
            set_off_line_servers.erase(it++);
            MAMC_INFO << *it << " index in online";
        }
    }
}

void MultiAddressMessageChannel::setAddressOffline(const std::string& remote_address) {
    if(!map_url_node_id.count(remote_address)) return;
        //get the server index
    uint32_t index = map_url_node_id[remote_address].feeder_index;
    service_feeder.setURLOffline(index);
    std::set<uint32_t>::iterator it = set_off_line_servers.find(index);
    if(it == set_off_line_servers.end()) {
        set_off_line_servers.insert(index);
    }
    MAMC_INFO << remote_address << " in offline";
}

void MultiAddressMessageChannel::setAddressOnline(const std::string& remote_address) {
    if(!map_url_node_id.count(remote_address)) return;
        //get the server index
    uint32_t index = map_url_node_id[remote_address].feeder_index;
    service_feeder.setURLOnline(index);
    std::set<uint32_t>::iterator it = set_off_line_servers.find(index);
    if(it != set_off_line_servers.end()) {
        set_off_line_servers.erase(it);
    }
    MAMC_INFO << remote_address << " in online";
}

const CNetworkAddressInfo& MultiAddressMessageChannel::getRemoteAddressInfo(const std::string& remote_address) {
    return map_url_node_id[remote_address];
}

    //
void MultiAddressMessageChannel::addNode(const CNetworkAddress& node_address) {
    if(map_url_node_id.count(node_address.ip_port) > 0) {
            //node already present
        return;
    }
        //! add address to the feeder
    uint32_t feeder_index = service_feeder.addURL(chaos::common::network::URL(node_address.ip_port));
        //!! add node address info to the map that associate the feeder index to the network node address
    map_url_node_id.insert(make_pair(node_address.ip_port, CNetworkAddressInfo(feeder_index,
                                                                               node_address)));
}

    //
void MultiAddressMessageChannel::removeNode(const CNetworkAddress& node_address) {
    if(map_url_node_id.count(node_address.ip_port) == 0) {
            //node not present
        return;
    }
    const CNetworkAddressInfo& addr_info = map_url_node_id[node_address.ip_port];
    service_feeder.removeURL(addr_info.feeder_index);
    map_url_node_id.erase(node_address.ip_port);
}

    //! remove all configured node
void MultiAddressMessageChannel::removeAllNode() {
    service_feeder.clear(true);
}

    //
void  MultiAddressMessageChannel::disposeService(void *service_ptr) {
    MMCFeederService *service = static_cast<MMCFeederService*>(service_ptr);
    map_url_node_id.erase(service->ip_port);
    if(service) delete(service);
}

    //
void* MultiAddressMessageChannel::serviceForURL(const URL& url,
                                                uint32_t service_index) {
    MMCFeederService *service = new MMCFeederService(url.getURL());
    return static_cast<void*>(service);
}

    //! get the rpc published host and port
void MultiAddressMessageChannel::getRpcPublishedHostAndPort(std::string& rpc_published_host_port) {
    MessageChannel::getRpcPublishedHostAndPort(rpc_published_host_port);
}

    //! send a message
void MultiAddressMessageChannel::sendMessage(const std::string& action_domain,
                                             const std::string& action_name,
                                             CDataWrapper *message_pack,
                                             bool on_this_thread) {
    MMCFeederService *service =  static_cast<MMCFeederService*>(service_feeder.getService());
    if(service) {
        MessageChannel::sendMessage(service->ip_port,
                                    action_domain,
                                    action_name,
                                    message_pack,
                                    on_this_thread);
        DEBUG_CODE(MAMC_DBG << "Sent message to:" << service->ip_port;)
    }

}

    //!send an rpc request to a remote node
std::auto_ptr<MessageRequestFuture> MultiAddressMessageChannel::_sendRequestWithFuture(const std::string& action_domain,
                                                                                       const std::string& action_name,
                                                                                       CDataWrapper *request_pack,
                                                                                       std::string& used_remote_address) {
    std::auto_ptr<MessageRequestFuture> result;
    MMCFeederService *service =  static_cast<MMCFeederService*>(service_feeder.getService());
    bool has_been_found_a_server = (service!=NULL);
    if(has_been_found_a_server) {
        result = MessageChannel::sendRequestWithFuture((used_remote_address = service->ip_port),
                                                       action_domain,
                                                       action_name,
                                                       request_pack);
        DEBUG_CODE(MAMC_DBG << "Sent request to:" << used_remote_address;)
    } else {
        used_remote_address.clear();
    }
    return result;
}

    //!send an rpc request to a remote node
std::auto_ptr<MultiAddressMessageRequestFuture> MultiAddressMessageChannel::sendRequestWithFuture(const std::string& action_domain,
                                                                                                  const std::string& action_name,
                                                                                                  CDataWrapper *request_pack,
                                                                                                  int32_t request_timeout) {
    retryOfflineServer(true);
    return std::auto_ptr<MultiAddressMessageRequestFuture>(new MultiAddressMessageRequestFuture(this,
                                                                                                action_domain,
                                                                                                action_name,
                                                                                                request_pack,
                                                                                                request_timeout));
}

