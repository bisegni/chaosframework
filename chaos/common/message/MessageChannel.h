/*
 *	MessageChannel.h
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
#ifndef ChaosFramework_MessageChannel_h
#define ChaosFramework_MessageChannel_h

#include <map>
#include <string>
#include <vector>

#include <chaos/common/message/MessageRequestFuture.h>

#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/action/DeclareAction.h>
#include <chaos/common/exception/CException.h>
#include <chaos/common/thread/MultiKeyObjectWaitSemaphore.h>
#include <chaos/common/network/CNodeNetworkAddress.h>
#include <chaos/common/network/NetworkBroker.h>

#include <boost/smart_ptr.hpp>
#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <boost/thread/future.hpp>
#include <boost/atomic.hpp>
namespace chaos {
	namespace common {
		namespace message {

			/*! \name Check Request Result Macro
			 These macros are used to check the result of a syncronous request operation.
			 */
			/*! \{ */

			/*! Check for delay error or application error */
#define CHECK_TIMEOUT_AND_RESULT_CODE(x,e) \
e = getLastErrorCode()

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
            typedef boost::shared_ptr<common::data::CDataWrapper>                                                       FuturePromiseData;
            typedef boost::promise< FuturePromiseData >                                                             MessageFuturePromise;
            typedef map<chaos::common::utility::atomic_int_type, boost::shared_ptr<MessageFuturePromise> >              MapPromises;
            typedef map<chaos::common::utility::atomic_int_type, boost::shared_ptr<MessageFuturePromise> >::iterator    MapPromisesIterator;
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
                
                //! Message broker associated with the channel instance
                NetworkBroker *broker;
                
				//! atomic int for request id
                boost::atomic<uint32_t> channel_request_id_counter;

				//! Mutex for managing the maps manipulation
				boost::mutex mutext_answer_managment;

                MapPromises map_request_id_promises;

				/*!
				 Initialization phase of the channel
				 */
				virtual void init() throw(CException);

				/*!
				 Initialization phase of the channel
				 */
				virtual void deinit() throw(CException);

			protected:
                /*!
                 Called when a response to a request is received, it will manage the search of
                 the apprioriate handler for the request
                 */
                virtual chaos::common::data::CDataWrapper* response(common::data::CDataWrapper *reposnse_data,
                                                                    bool& detach);

                //! the domain associate to this domain for get the answers
                std::string channel_reponse_domain;
                
				//!last error domain
				std::string last_error_domain;

				//!last error message
				std::string last_error_message;

				//!error code
				int32_t last_error_code;

				/*!
				 Private constructor called by NetworkBroker
				 */
				MessageChannel(NetworkBroker *_broker);

				/*!
				 Private destructor called by NetworkBroker
				 */
				virtual ~MessageChannel();

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

                    //! Sena an rpc message to a remote node
				/*!
				 send a message
                 \param remote_host is the host:port string that identify the remote server where send the rpc message
                 \param node_id id of the remote node within a network broker interface
                 \param action_name the name of the action to call
				 \param message_pack the data to send, the pointer is not deallocated and i scopied into the pack
                 \param on_this_thread notify when the message need to be sent syncronously or in async  way
				 */
				void sendMessage(const std::string& remote_host,
                                 const std::string& node_id,
                                 const std::string& action_name,
                                 chaos::common::data::CDataWrapper * message_pack,
                                 bool on_this_thread = false);


                    //!send an rpc request to a remote node
                /*!
                 send a syncronous request and can wait for a determinated number of milliseconds the answer. If it has not
                 been received the method return with a NULL pointer
                 \param remote_host is the host:port string that identify the remote server where send the rpc request
                 \param node_id id of the node into remote chaos rpc system
                 \param action_name name of the actio to call
                 \param request_pack the data to send, the pointer is not deallocated and i scopied into the pack
                 \param millisec_to_wait waith the response for onli these number of millisec then return
                 \param on_this_thread notify when the message need to be sent syncronously or in async  way
                 \return the answer of the request, a null value mean that the wait time is expired
                 */
                common::data::CDataWrapper* sendRequest(const std::string& remote_host,
                                                        const std::string& node_id,
                                                        const std::string& action_name,
                                                        chaos::common::data::CDataWrapper *request_pack,
                                                        int32_t millisec_to_wait=-1,
                                                        bool async = false,
                                                        bool on_this_thread = false);

                    //!send an rpc request to a remote node
                /*!
                 send a syncronous request and can wait for a determinated number of milliseconds the answer. If it has not
                 been received the method return with a NULL pointer
                 \param remote_host is the host:port string that identify the remote server where send the rpc request
                 \param node_id id of the node into remote chaos rpc system
                 \param action_name name of the actio to call
                 \param request_pack the data to send, the pointer is not deallocated and i scopied into the pack
                 \return the future object to inspec and whait the result
                 */
                virtual std::auto_ptr<MessageRequestFuture> sendRequestWithFuture(const std::string& remote_host,
                                                                                  const std::string& node_id,
                                                                                  const std::string& action_name,
                                                                                  chaos::common::data::CDataWrapper *request_pack = NULL,
                                                                                  bool on_this_thread = false);

                //! get the rpc published host and port
                void getRpcPublishedHostAndPort(std::string& rpc_published_host_port);
			};
		}
	}
}
#endif
