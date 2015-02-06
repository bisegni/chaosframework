/*
 *	MessageChannel.h
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
#ifndef ChaosFramework_MessageChannel_h
#define ChaosFramework_MessageChannel_h

#include <map>
#include <string>
#include <vector>

#include <chaos/common/utility/Atomic.h>
#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/action/DeclareAction.h>
#include <chaos/common/exception/CException.h>
#include <chaos/common/thread/MultiKeyObjectWaitSemaphore.h>
#include <chaos/common/network/CNodeNetworkAddress.h>
#include <chaos/common/network/NetworkBroker.h>
#include <boost/function.hpp>
#include <boost/thread.hpp>

namespace chaos {
	namespace common {
		namespace message {
			
			/*! \name Check Request Result Macro
			 These macros are used to check the result of a syncronous request operation.
			 */
			/*! \{ */
			
			/*! Check for delay error or application error */
#define CHECK_TIMEOUT_AND_RESULT_CODE(x,e) \
if(!x.get()) {\
e = ErrorCode::EC_TIMEOUT;\
} else if(x->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE)) {\
e = x->getInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE);\
}
			
#define MC_PARSE_CDWPTR_RESULT(x) \
if(x==NULL) {\
last_error_code = ErrorCode::EC_TIMEOUT;\
} else {\
if(x->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE)) last_error_code = x->getInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE);\
if(x->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_MESSAGE)) last_error_message = x->getStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_MESSAGE);\
if(x->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_DOMAIN)) last_error_domain = x->getStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_DOMAIN);\
}
			
#define MC_GET_RESULT_DATA(x)\
if(x->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE)) x->getCSDataValue(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE);
			
			/*! \} */
			
			typedef void (*MessageHandler)(chaos::common::utility::atomic_int_type, common::data::CDataWrapper*);
			
			
			//! MessageChannel
			/*!
			 This is the basic channel for comunicate with other chaos rpc server. It can be instantiated only by
			 NetworkBroker. It can send a message or a request. Message is the async forward data to domain/action hosted on remote rpc server and no answer is waited.
			 Request is two pahse message, first a message(the reqeust) is sent to  domain/action on remote rpc server, then an aswer is waited this step can
			 be achieved in two way:
			 1 - Async mode, an handler must be given on request call
			 2 - Sync mode, wait until the answer is received, or can be set a number of milliseconds to wait for.
			 */
			class MessageChannel : public DeclareAction {
				friend class chaos::common::network::NetworkBroker;
				
				//! atomic int for request id
				chaos::common::utility::atomic_int_type channelRequestIDCounter;
				
				//! remote host where send the message and request
				
				string remoteNodeAddress;
				
				//! channel id (action domain)
				string channelID;
				
				//!multi key semaphore for manage the return of the action and result association to the reqeust id
				MultiKeyObjectWaitSemaphore<chaos::common::utility::atomic_int_type,common::data::CDataWrapper*> sem;
				
				//! Mutex for managing the maps manipulation
				boost::shared_mutex mutext_answer_managment;
				
				//!map to async request and handler
				MessageHandler response_handler;
				
				//!map to sync request and result
				map<chaos::common::utility::atomic_int_type, common::data::CDataWrapper* > response_id_sync_map;
				
				/*!
				 Initialization phase of the channel
				 */
				virtual void init() throw(CException);
				
				/*!
				 Initialization phase of the channel
				 */
				virtual void deinit() throw(CException);
				
				/*!
				 \brief called when a response to a request is received, it will manage thesearch of
				 hanlder specified for request id request
				 */
				chaos_data::CDataWrapper* response(common::data::CDataWrapper*, bool&);
				
				/*!
				 Set the reqeust id into the CDataWrapper
				 \param requestPack the request pack to send
				 \return the unique request id
				 */
				chaos::common::utility::atomic_int_type prepareRequestPackAndSend(bool async,
																				  const char * const nodeID,
																				  const char * const actionName,
																				  CDataWrapper *requestPack,
																				  bool onThisThread);
				
			protected:
				//! Message broker associated with the channel instance
				NetworkBroker *broker;
				
				//!last error domain
				std::string last_error_domain;
				
				//!last error message
				std::string last_error_message;
				
				//!error code
				int32_t last_error_code;
				
				/*!
				 Private constructor called by NetworkBroker
				 \param _borker the broker instance that has created the channel
				 \param _remote_host the remote host where send the message
				 */
				MessageChannel(NetworkBroker *_broker,
							   const std::string& _remote_host);
				
				/*!
				 Private constructor called by NetworkBroker
				 */
				MessageChannel(NetworkBroker *_broker);
				
				/*!
				 Private destructor called by NetworkBroker
				 */
				virtual ~MessageChannel();
				
				/*!
				 Update the address of the channel
				 */
				void setRemoteNodeAddress(const std::string& remote_addr);
				
				/*!
				 Return the broker for that channel
				 */
				NetworkBroker * getBroker();
				
			public:
                //! return last sendxxx error code
				int32_t getLastErrorCode();
                
                //! return last sendxxx error message
				const std::string& getLastErrorMessage();
                
				//! return last sendxxx error domain
				const std::string& getLastErrorDomain();
				
				/*!
				 \brief send a message
                 \param node_id id of the remote node within a network broker interface
                 \param action_name the name of the action to call
				 \param message_pack the data to send, the pointer is not deallocated and i scopied into the pack
                 \param on_this_thread notify when the message need to be sent syncronously or in async  way
				 */
				void sendMessage(const char * const node_id,
                                 const char * const action_name,
                                 CDataWrapper * const message_pack,
                                 bool on_this_thread = false);
				
				/*!
				 \brief Set the handler for manage the rpc answer
				 \param async_handler the handler to be used
				 */
				void setHandler(MessageHandler async_handler);
				
				/*!
				 remove the handler
				 */
				void clearHandler();
				
				/*!
				 Poll for see if the response is arrived
				 */
				common::data::CDataWrapper* pollAnswer(chaos::common::utility::atomic_int_type request_id, uint32_t millisec_to_wait = 0);
				
				/*!
				 \brief send a syncronous request and can wait for a determinated number of milliseconds the answer. If it has not
				 been received the method return with a NULL pointer
				 \param nodeID id of the node into remote chaos rpc system
				 \param actionName name of the actio to call
				 \param requestPack the data to send, the pointer is not deallocated and i scopied into the pack
				 \param millisecToWait waith the response for onli these number of millisec then return
				 \return the answer of the request, a null value mean that the wait time is expired
				 */
				common::data::CDataWrapper* sendRequest(const char * const nodeID,
														const char * const actionName,
														CDataWrapper * const requestPack,
														uint32_t millisecToWait=0,
														bool async = false,
														bool onThisThread = false);
			};
		}
	}
}
#endif