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
        
        void forwadSubmissionResult(NFISharedPtr message_info,
                                    chaos::common::data::CDWUniquePtr submission_result);
        /*!
         Forward to dispatcher the error durngi the forwarding of the request message
         */
        void forwadSubmissionResultError(const std::string& channel_node_id,
                                         uint32_t message_request,
                                         chaos::common::data::CDWUniquePtr submission_result);
        
        /*!
         Forward to dispatcher the error durngi the forwarding of the request message
         */
        void forwadSubmissionResultError(NFISharedPtr message_info,
                                         int32_t error_code,
                                         const std::string& error_message,
                                         const std::string& error_domain);
    public:
        /*!
         Constructor di default per i
         */
        RpcClient(const std::string& alias);
        virtual ~RpcClient();
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
