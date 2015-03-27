/*	
 *	RpcClient.h
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
#ifndef ChaosFramework_RPCClient_h
#define ChaosFramework_RPCClient_h

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include <chaos/common/global.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/exception/CException.h>
#include <chaos/common/configuration/GlobalConfiguration.h>
#include <chaos/common/utility/StartableService.h>
#include <chaos/common/utility/NamedService.h>
#include <chaos/common/rpc/RpcMessageForwarder.h>
#include <chaos/common/rpc/RpcServerHandler.h>

using namespace chaos::common::network;
namespace chaos_data = chaos::common::data;

namespace chaos {
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
		friend class chaos::common::network::NetworkBroker;
        //! handler to the dispatcher to forward error on data forwarding
        RpcServerHandler *server_handler;
    protected:
        /*!
         init the rpc adapter
         */
        virtual void init(void*) throw(CException) = 0;
        
        /*!
         start the rpc adapter
         */
        virtual void start() throw(CException) = 0;
        
        /*!
         deinit the rpc adapter
         */
        virtual void deinit() throw(CException) = 0;
        
        /*!
         Forward to dispatcher the error durngi the forwarding of the request message
         */
        void forwadSubmissionResultError(const std::string& channel_node_id,
                                         uint32_t message_request,
                                         chaos::common::data::CDataWrapper *submission_result);
        
        /*!
         Forward to dispatcher the error durngi the forwarding of the request message
         */
        void forwadSubmissionResultError(const std::string& channel_node_id,
                                         uint32_t message_request,
                                         int32_t error_code,
                                         const std::string& error_message,
                                         const std::string& error_domain);
    public:
        /*!
         Constructor di default per i
         */
        RpcClient(const std::string& alias);
    };
}
#endif
