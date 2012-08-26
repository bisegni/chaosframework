/*	
 *	MsgPackClient.h
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
#ifndef ChaosFramework_MsgPackClient_h
#define ChaosFramework_MsgPackClient_h
#include "RpcClient.h"
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/pqueue/ChaosProcessingQueue.h>
#include <chaos/common/exception/CException.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>

#include <msgpack/rpc/client.h>
#include <msgpack/rpc/client.h>
#include <msgpack/rpc/session_pool.h>

#include <string>

namespace chaos {
    using namespace std;
    
    
    /*
     Class that manage the MessagePack message send.
     */
     REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY(MsgPackClient, RpcClient), public CObjectProcessingQueue<RpcMessageForwardInfo> {
            //messagepack connection pooling
        msgpack::rpc::session_pool *connectionPolling;
    protected:
        virtual void processBufferElement(RpcMessageForwardInfo*, ElementManagingPolicy&) throw(CException);
        
         /*
          init the rpc adapter
          */
         void init(CDataWrapper*) throw(CException);
         
         /*
          start the rpc adapter
          */
         void start() throw(CException);
         
         /*
          deinit the rpc adapter
          */
         void deinit() throw(CException);
         
    public:
         MsgPackClient(string *alias);
         ~MsgPackClient();
        
        
        /*
         Submit the message to be send to a certain ip, the datawrapper must contains
         the key CS_CMDM_REMOTE_HOST_IP
         */
        bool submitMessage(string& destinationIpAndPort, CDataWrapper *message, bool onThisThread=false) throw(CException);
    };
}
#endif
