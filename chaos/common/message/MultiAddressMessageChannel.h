/*
 *	MultiAddressMessageChannel.h
 *	!CHOAS
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
#ifndef __CHAOSFramework__MultiAddressMessageChannel__
#define __CHAOSFramework__MultiAddressMessageChannel__
#include <chaos/common/message/MessageChannel.h>
#include <chaos/common/message/MultiAddressMessageRequestFuture.h>
#include <chaos/common/network/URLServiceFeeder.h>

#include <map>
#include <string>
#include <set>
namespace chaos {
    namespace common {
        namespace network {
            class NetworkBroker;
        }
        namespace message {

                //!structure for the association with the feeder index and the node address
            struct CNetworkAddressInfo {
                    //! index that indetify the address node within the feeder
                uint32_t feeder_index;

                    //! the node address
                chaos::common::network::CNetworkAddress network_address;

                CNetworkAddressInfo(){};
                CNetworkAddressInfo(uint32_t _feeder_index,
                                    const chaos::common::network::CNetworkAddress& _network_address):
                feeder_index(_feeder_index),
                network_address(_network_address){};
            };

                //! feeder service structure
            struct MMCFeederService {
                    //url of the service
                std::string ip_port;
                MMCFeederService(const std::string& _ip_port):
                ip_port(_ip_port){};
            };


            //! Multinode message channel
            /*!
             This channel permit to scale the message and request on many different server in a
             fail over or roundrobin way. Obviously this kind of channel can be used thinking 
             that the service that the channel needs to comunicate with, is the same. In different url the
             service could have a different node id (internall to the network broker). So a map
             is used to mantains the node_id with the url.
             */
            class MultiAddressMessageChannel:
            private MessageChannel,
            private chaos::common::network::URLServiceFeederHandler {
                friend class chaos::common::network::NetworkBroker;
                friend class chaos::common::message::MultiAddressMessageRequestFuture;

                uint64_t last_retry;
                    //!list of the offline server index
                /*!
                 during the wait the server that are in error are put offline, when
                 a new wait is executed all offline server are put online to try
                 if the are available
                 */
                std::set<uint32_t> set_off_line_servers;

                //! url manager
                chaos::common::network::URLServiceFeeder service_feeder;

                    //! default constructor
                MultiAddressMessageChannel(chaos::common::network::NetworkBroker *message_broker);

                //!Base constructor
                /*!
                 Perform the multinode node network construction
                 \param message_broker the broker used by this channel
                 \param node_address a list of node that the channel used for the data forwarding
                 */
                MultiAddressMessageChannel(chaos::common::network::NetworkBroker *message_broker,
                                        chaos::common::network::CNetworkAddress& node_address);
                //!Base constructor
                /*!
                 Perform the multinode node network construction
                 \param message_broker the broker used by this channel
                 \param node_address a list of node that the channel used for the data forwarding
                 */
                MultiAddressMessageChannel(chaos::common::network::NetworkBroker *message_broker,
                                        const std::vector<chaos::common::network::CNetworkAddress>& node_address);
                
                ~MultiAddressMessageChannel();

                    //!internal send request method
                /*!
                 send an rpc request with url feeder rule for servec choice and return the future.
                 IMPORTANT: the memory for request data is internally managed by the channel for permitting the
                 retransmission on timeout
                 \param action_name the name of the action to call
                 \param request_pack the request pack to forward to the action
                 \return the future of the request or null if no server has been found
                 */
                std::auto_ptr<MessageRequestFuture> _sendRequestWithFuture(const std::string& action_domain,
                                                                           const std::string& action_name,
                                                                           chaos::common::data::CDataWrapper *request_pack,
                                                                           std::string& used_remote_address);
            protected:
                //! the map link the url with the ndoe id
                std::map<std::string, CNetworkAddressInfo> map_url_node_id;
                
                void  disposeService(void *service_ptr);
                void* serviceForURL(const chaos::common::network::URL& url,
                                    uint32_t service_index);

                void retryOfflineServer(bool force = false);
            public:
                const CNetworkAddressInfo& getRemoteAddressInfo(const std::string& remote_address);
                //! add a new node to the channel
                void addNode(const chaos::common::network::CNetworkAddress& node_address);
                //! remove a node from the channel
                void removeNode(const chaos::common::network::CNetworkAddress& node_address);
                //! get the rpc published host and port
                void getRpcPublishedHostAndPort(std::string& rpc_published_host_port);
                /*!
                 \brief send a message
                 \param node_id id of the remote node within a network broker interface
                 \param action_name the name of the action to call
                 \param message_pack the data to send, the pointer is not deallocated and i scopied into the pack
                 \param on_this_thread notify when the message need to be sent syncronously or in async  way
                 */
                void sendMessage(const std::string& action_domain,
                                 const std::string& action_name,
                                 chaos::common::data::CDataWrapper * const message_pack,
                                 bool on_this_thread = false);
                //!send an rpc request to a remote node
                /*!
                 send an rpc request with url feeder rule for servec choice and return the future.
                 IMPORTANT: the memory for request data is internally managed by the channel for permitting the
                 retransmission on timeout
                 \param action_name the name of the action to call
                 \param request_pack the request pack to forward to the action
                 \return the future of the request or null if no server has been found
                 */
                std::auto_ptr<MultiAddressMessageRequestFuture> sendRequestWithFuture(const std::string& action_domain,
                                                                                      const std::string& action_name,
                                                                                      chaos::common::data::CDataWrapper *request_pack,
                                                                                      int32_t request_timeout = 1000);

                void setAddressOffline(const std::string& remote_address);
                void setAddressOnline(const std::string& remote_address);
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__MultiAddressMessageChannel__) */
