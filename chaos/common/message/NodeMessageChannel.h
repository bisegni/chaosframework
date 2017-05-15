/*
 *	NodeMessageChannel.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
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
#ifndef CHAOSFramework_NodeMessageChannel_h
#define CHAOSFramework_NodeMessageChannel_h
#include <chaos/common/message/MessageChannel.h>
#include <chaos/common/network/CNodeNetworkAddress.h>

#include <vector>

namespace chaos {
	namespace common {
		namespace network {
			class NetworkBroker;
		}
		namespace message {
                //using namespace std;
                //using namespace chaos::common::network;
			//! Message Channel constructed with a node address
			/*!
			 This represent a subclass of MessageChannel that is addess with an instance of
			 CNodeNetworkAddress ans information for the endpoint
			 */
			class NodeMessageChannel:
            protected MessageChannel {
				friend class chaos::common::network::NetworkBroker;

			protected:
				//! node address for metadata server
                std::auto_ptr<chaos::common::network::CNodeNetworkAddress> nodeAddress;
				
			public:
				//!Base constructor
				/*!
				 Perform the node notwork construction
				 \param msgBroker the broker used by this channel
				 \param mdsIpAddress the address of metdataserver
				 */
                NodeMessageChannel(chaos::common::network::NetworkBroker *msgBroker,
                                   chaos::common::network::CNodeNetworkAddress *_nodeAddress,
                                   MessageRequestDomainSHRDPtr _new_message_request_domain = MessageRequestDomainSHRDPtr(new MessageRequestDomain()));
				
                virtual ~NodeMessageChannel();
				
				/*!
				 Set a new addres for the channel
				 \param _nodeAddress the pointer to a strcture of type CNodeNetworkAddress. The mermory is managed internally by the channel
				 */
                void setNewAddress(chaos::common::network::CNodeNetworkAddress *_nodeAddress);
				
                //! get the rpc published host and port
                void getRpcPublishedHostAndPort(std::string& rpc_published_host_port);
                
                /*!
                 \brief send a message
                 \param node_id id of the remote node within a network broker interface
                 \param action_name the name of the action to call
                 \param message_pack the data to send, the pointer is not deallocated and i scopied into the pack
                 \param on_this_thread notify when the message need to be sent syncronously or in async  way
                 */
                void sendMessage(const std::string& node_id,
                                 const std::string& action_name,
                                 chaos::common::data::CDataWrapper * const message_pack);
                /*!
                 \brief send a syncronous request and can wait for a determinated number of milliseconds the answer. If it has not
                 been received the method return with a NULL pointer
                 \param node_id id of the node into remote chaos rpc system
                 \param action_name name of the actio to call
                 \param request_pack the data to send, the pointer is not deallocated and i scopied into the pack
                 \param millisec_to_wait waith the response for onli these number of millisec then return
                 \param on_this_thread notify when the message need to be sent syncronously or in async  way
                 \return the answer of the request, a null value mean that the wait time is expired
                 */
                common::data::CDataWrapper* sendRequest(const std::string& node_id,
                                                        const std::string& action_name,
                                                        chaos::common::data::CDataWrapper *request_pack,
                                                        int32_t millisec_to_wait=-1);
                
                //!send an rpc request to a remote node
                std::auto_ptr<MessageRequestFuture> sendRequestWithFuture(const std::string& node_id,
                                                                          const std::string& action_name,
                                                                          chaos::common::data::CDataWrapper *request_pack);
                
                //!Send a request for receive RPC information
                /*!
                 RPC information contains the alive state of the node within rpc dispacher on the backend size
                 and the size of the queued actions
                 */
                std::auto_ptr<MessageRequestFuture> checkRPCInformation(const std::string& node_id);
                
                //!Send a request for an echo test
                std::auto_ptr<MessageRequestFuture> echoTest(chaos::common::data::CDataWrapper *echo_data);
                
                //! return last sendxxx error code
                int32_t getLastErrorCode();
                
                //! return last sendxxx error message
                const std::string& getLastErrorMessage();
                
                //! return last sendxxx error domain
                const std::string& getLastErrorDomain();
			};
		}
	}
}
#endif
