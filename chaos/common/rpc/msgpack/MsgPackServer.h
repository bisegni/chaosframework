/*	
 *	MsgPackServer.h
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

#ifndef MsgPackServer_H
#define MsgPackServer_H
#pragma GCC diagnostic ignored "-Woverloaded-virtual"

#include <boost/shared_ptr.hpp>
#include <msgpack/rpc/server.h>


#include <chaos/common/exception/CException.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/rpc/RpcServer.h>

namespace chaos{
    
    
    using namespace boost;
    namespace rpc {
        using namespace msgpack;
        using namespace msgpack::rpc;
    }  
    
    /*
     Class that implement the Chaos RPC adapter for msgpack protocoll
     */
    REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY(MsgPackServer, RpcServer), public rpc::dispatcher {
        
        int threadNumber;
            //msgpack server instance
        rpc::server msgpackServer;
        
    public:
        typedef rpc::request request;
        
        MsgPackServer(string alias);
        
        virtual ~MsgPackServer();
        
            /*
             init the rpc adapter
             */
        void init(void*) throw(CException);
            /*
             start the rpc adapter
             */
        void start() throw(CException);
        
        /*
         start the rpc adapter
         */
        void stop() throw(CException);
        
            /*
             deinit the rpc adapter
             */
        void deinit() throw(CException);
        
        /*
         
         */
        void dispatch(request req);
    };
}
#endif

