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
#ifndef ChaosFramework_MessageChannel_h
#define ChaosFramework_MessageChannel_h

#include <map>
#include <string>
#include <vector>

#include <chaos/common/message/MessageRequestFuture.h>

#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/utility/SafeAsyncCall.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/exception/exception.h>
#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/network/CNodeNetworkAddress.h>
#include <chaos/common/message/MessageRequestDomain.h>
#include <chaos/common/thread/MultiKeyObjectWaitSemaphore.h>

#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <boost/thread/future.hpp>
#include <boost/atomic.hpp>

namespace chaos {
    namespace common {
        namespace message {
            
            /*! \name Check Request Result Macro
             These macros are used to check the result of a synCronus request operation.
             */
            /*! \{ */
            
            /*! Check for delay error or application error */
#define CHECK_TIMEOUT_AND_RESULT_CODE(x, e) \
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

            //! MessageChannel
            /*!
             This is the basic channel for comunicate with other chaos rpc server. It can be instantiated only by
             NetworkBroker. It can send a message or a request. Message is the async forward data to domain/action hosted on remote rpc server and no answer is waited.
             Request is two pahse message, first a message(the reqeust) is sent to  domain/action on remote rpc server, then an aswer is waited this step can
             be achieved in two way:
             1 - Async mode, an handler must be given on request call
             2 - Sync mode, wait until the answer is received, or can be set a number of milliseconds to wait for.
             */
            class MessageChannel {
                friend class chaos::common::network::NetworkBroker;
                
                //! Message broker associated with the channel instance
                NetworkBroker *broker;
                
                //! the use of shared pointer of MessageRequestDomain permit to share the same response domain to different messahe channel
                ChaosSharedPtr<MessageRequestDomain> message_request_domain;

                    //! unique channel identifier
                std::string channel_uuid;
                PromisesHandlerSharedPtr safe_promises_handler_caller;
                
                void _callHandler(const FuturePromiseData& response_data);
            protected:
                
                //!last error domain
                std::string last_error_domain;
                
                //!last error message
                std::string last_error_message;
                
                //!error code
                int32_t last_error_code;
                
                /*!
                 Private constructor called by NetworkBroker
                 */
                MessageChannel(NetworkBroker *_broker,
                               MessageRequestDomainSHRDPtr _new_message_request_domain = MessageRequestDomainSHRDPtr(new MessageRequestDomain()));
                
                /*!
                 Private destructor called by NetworkBroker
                 */
                virtual ~MessageChannel();
                
                /*!
                 Return the broker for that channel
                 */
                NetworkBroker *getBroker();
                
                //!promises handler for request results introspection
                /*!
                 Repsonse data is received befor it is sent to the metadata server
                 */
                virtual void requestPromisesHandler(const FuturePromiseData& response_data);
                
                /*!
                 Initialization phase of the channel
                 */
                virtual void init() throw(CException);
                
                /*!
                 Initialization phase of the channel
                 */
                virtual void deinit() throw(CException);
            public:
                //! return last sendxxx error code
                virtual int32_t getLastErrorCode();
                
                //! return last sendxxx error message
                virtual const std::string& getLastErrorMessage();
                
                //! return last sendxxx error domain
                virtual const std::string& getLastErrorDomain();

                const std::string& getChannelUUID();

                    //! return the channel request domain
                ChaosSharedPtr<MessageRequestDomain> getMessageRequestDomain();

                
                //! Sena an rpc message to a remote node
                /*!
                 send a message
                 \param remote_host is the host:port string that identify the remote server where send the rpc message
                 \param node_id id of the remote node within a network broker interface
                 \param action_name the name of the action to call
                 \param message_pack the data to send, the pointer is not deallocated and i scopied into the pack
                 \param on_this_thread notify when the message need to be sent synCronusly or in async  way
                 */
                void sendMessage(const std::string &remote_host,
                                 const std::string &node_id,
                                 const std::string &action_name,
                                 chaos::common::data::CDWUniquePtr message_pack);
                
                
                //!send an rpc request to a remote node
                /*!
                 send a synCronus request and can wait for a determinated number of milliseconds the answer. If it has not
                 been received the method return with a NULL pointer
                 \param remote_host is the host:port string that identify the remote server where send the rpc request
                 \param node_id id of the node into remote chaos rpc system
                 \param action_name name of the actio to call
                 \param request_pack the data to send, the pointer is not deallocated and i scopied into the pack
                 \param millisec_to_wait waith the response for onli these number of millisec then return
                 \param on_this_thread notify when the message need to be sent synCronusly or in async  way
                 \return the answer of the request, a null value mean that the wait time is expired
                 */
                common::data::CDWUniquePtr sendRequest(const std::string &remote_host,
                                                       const std::string &node_id,
                                                       const std::string &action_name,
                                                       chaos::common::data::CDWUniquePtr request_pack,
                                                       int32_t millisec_to_wait = -1);
                
                //!send an rpc request to a remote node
                /*!
                 send a synCronus request and can wait for a determinated number of milliseconds the answer. If it has not
                 been received the method return with a NULL pointer
                 \param remote_host is the host:port string that identify the remote server where send the rpc request
                 \param node_id id of the node into remote chaos rpc system
                 \param action_name name of the actio to call
                 \param request_pack the data to send, the pointer is not deallocated and i scopied into the pack
                 \return the future object to inspec and whait the result
                 */
                virtual ChaosUniquePtr<MessageRequestFuture> sendRequestWithFuture(const std::string &remote_host,
                                                                                  const std::string &node_id,
                                                                                  const std::string &action_name,
                                                                                  chaos::common::data::CDWUniquePtr request_pack);
                
                
                //!Send a request for receive RPC information
                /*!
                 RPC information contains the alive state of the node within rpc dispacher on the backend size
                 and the size of the queued actions
                 */
                ChaosUniquePtr<MessageRequestFuture> checkRPCInformation(const std::string &remote_host,
                                                                        const std::string &node_id);
                
                //!Send a request for an echo test
                ChaosUniquePtr<MessageRequestFuture> echoTest(const std::string &remote_host,
                                                             chaos::common::data::CDWUniquePtr echo_data);
                
                //! get the rpc published host and port
                void getRpcPublishedHostAndPort(std::string &rpc_published_host_port);
            };
        }
    }
}
#endif
