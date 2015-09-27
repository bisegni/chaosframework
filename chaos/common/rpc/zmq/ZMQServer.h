/*	
 *	ZMQServer.h
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

#ifndef CHAOSFramework_ZMQServer_h
#define CHAOSFramework_ZMQServer_h
#pragma GCC diagnostic ignored "-Woverloaded-virtual"

#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <zmq.h>

#include <chaos/common/rpc/RpcServer.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>


namespace chaos {
    /*
     Class that implement the Chaos RPC adapter for 0mq protocoll
     */
    DECLARE_CLASS_FACTORY(ZMQServer, RpcServer)  {
        REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(ZMQServer)
        int threadNumber;
        void *zmqContext;
        std::stringstream bindStr;
        boost::thread_group threadGroup;
        bool runServer;
        
        ZMQServer(const std::string& alias);
        virtual ~ZMQServer();
    public:

        /*
         init the rpc adapter
         */
        void init(void *init_data) throw(CException);
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
        
            //server worker thread
        /*!
         Thread where data is received and managed
         */
        void executeOnThread();
    };
    
}
#endif
