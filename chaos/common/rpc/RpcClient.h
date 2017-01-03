/*
 *	RpcClient.h
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

#ifndef ChaosFramework_RPCClient_h
#define ChaosFramework_RPCClient_h

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include <chaos/common/global.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/exception/exception.h>
#include <chaos/common/configuration/GlobalConfiguration.h>
#include <chaos/common/utility/StartableService.h>
#include <chaos/common/utility/NamedService.h>
#include <chaos/common/rpc/RpcMessageForwarder.h>
#include <chaos/common/rpc/RpcServerHandler.h>

using namespace chaos::common::network;
namespace chaos_data = chaos::common::data;

namespace chaos {
#define RPC_CLIENT_SET_ERROR_OFFSET 100000
    
    /*!
     Define the information for send a message to some server
     */
    typedef struct {
        //! list of the server where to send data
        vector<string> remoteServers;
        //message to forward to remote server
        chaos_data::CDataWrapper *message;
    } MessageForwardingInfo;
    
    namespace common {
        namespace network {
            class NetworkBroker;
        }
    }
    
    /*!
     Abstract class for standard adapter method for permit, to CommandManager
     the correct initialization for the adapter instance
     */
    class RpcClient:
    public RpcMessageForwarder,
    public common::utility::StartableService,
    public common::utility::NamedService {
        //friend class chaos::common::network::NetworkBroker;
        //! handler to the dispatcher to forward error on data forwarding
        RpcServerHandler *server_handler;
    protected:
        
        bool syncrhonous_call;
        
        void forwadSubmissionResult(NetworkForwardInfo *message_info,
                                    chaos::common::data::CDataWrapper *submission_result);
        /*!
         Forward to dispatcher the error durngi the forwarding of the request message
         */
        void forwadSubmissionResultError(const std::string& channel_node_id,
                                         uint32_t message_request,
                                         chaos::common::data::CDataWrapper *submission_result);
        
        /*!
         Forward to dispatcher the error durngi the forwarding of the request message
         */
        void forwadSubmissionResultError(NetworkForwardInfo *message_info,
                                         int32_t error_code,
                                         const std::string& error_message,
                                         const std::string& error_domain);
    public:
        /*!
         Constructor di default per i
         */
        RpcClient(const std::string& alias);
        
        virtual void setServerHandler(RpcServerHandler *_server_handler);
        
        //! return the numebr of message that are waiting to be sent
        /*!
         driver can overload this method to return(if has any) the size
         of internal queue message
         */
        virtual uint64_t getMessageQueueSize();
    };
}
#endif
