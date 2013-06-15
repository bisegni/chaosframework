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
#include <chaos/common/rpc/RpcMessageForwarder.h>
namespace chaos {
    /*!
     Define the information for send a message to some server
     */
    typedef struct {
        //! listO of the server where to send data
        vector<string> remoteServers;
        //message to forward to remote server
        CDataWrapper *message;
    } MessageForwardingInfo;
    
    class NetworkBroker;
    
    /*!
     Abstract class for standard adapter method for permit, to CommandManager
     the correct initialization for the adapter instance
     */
    class RpcClient: public RpcMessageForwarder, chaos::utility::StartableService {
        friend class NetworkBroker;
        string *typeName;
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
         manage the call for the handler when nd error occours
         */
        inline void callErrorHandler(NetworkForwardInfo *fInfo, ErrorCode::ErrorCode eCode);
        
        /*!
         manage the call for the handler when the operation has ended
         */
        inline void callEndOpHandler(NetworkForwardInfo *fInfo);
    public:
        /*!
         Constructor di default per i
         */
        RpcClient(string *alias);
        
        /*!
         Return the adapter alias
         */
        const char * getName() const;
    };
}
#endif
