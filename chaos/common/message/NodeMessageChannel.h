/*
 *	NodeMessageChannel.h
 *	!CHOAS
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
			using namespace std;
			using namespace chaos::common::network;
			//! Message Channel constructed with a node address
			/*!
			 This represent a subclass of MessageChannel that is addess with an instance of
			 CNodeNetworkAddress ans information for the endpoint
			 */
			class NodeMessageChannel : protected MessageChannel {
				friend class chaos::common::network::NetworkBroker;
			protected:
				//! node address for metadata server
				auto_ptr<CNodeNetworkAddress> nodeAddress;
				
			public:
				//!Base constructor
				/*!
				 Perform the node notwork construction
				 \param msgBroker the broker used by this channel
				 \param mdsIpAddress the address of metdataserver
				 */
				NodeMessageChannel(NetworkBroker *msgBroker,  CNodeNetworkAddress *_nodeAddress) : MessageChannel(msgBroker) {
					CHAOS_ASSERT(_nodeAddress)
					setNewAddress(_nodeAddress);
				}
				
				virtual ~NodeMessageChannel(){
					//if(nodeAddress) delete nodeAddress;
				}
				
				/*!
				 Set a new addres for the channel
				 \param _nodeAddress the pointer to a strcture of type CNodeNetworkAddress. The mermory is managed internally by the channel
				 */
				void setNewAddress(CNodeNetworkAddress *_nodeAddress) {
					nodeAddress.reset(_nodeAddress);
					if(_nodeAddress)setRemoteNodeAddress(nodeAddress->ipPort);
				}
				
				/*!
				 \brief Expose the set handler api
				 */
				void setHandler(MessageHandler async_handler) {
					MessageChannel::setHandler(async_handler);
				}
				
				/*!
				 Expose the remove handler api
				 */
				void clearHandler() {
					MessageChannel::clearHandler();
				}
				
				/*!
				 Expose the poll api
				 */
				common::data::CDataWrapper* pollAnswer(atomic_int_type request_id, uint32_t millisec_to_wait = 0) {
					return MessageChannel::pollAnswer(request_id, millisec_to_wait);
				}
			};
		}
	}
}
#endif
