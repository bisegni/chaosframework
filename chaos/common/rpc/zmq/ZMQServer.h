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

#ifndef CHAOSFramework_ZMQServer_h
#define CHAOSFramework_ZMQServer_h
//#pragma GCC diagnostic ignored "-Woverloaded-virtual"

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
        int thread_number;
        void *zmq_context;
        std::stringstream bind_str;
        boost::thread_group thread_group;
        bool run_server;
        
        ZMQServer(const std::string& alias);
        virtual ~ZMQServer();
        //worker that process request in a separate thread
        void worker();
        
        int sendMessage(void *socket,
                        void *message_data,
                        size_t message_size,
                        bool more_to_send);
        
        int readMessage(void *socket,
                        std::string& buffer,
                        bool& has_next,
                        std::string *peer_ip = NULL);
    public:
        
        /*
         init the rpc adapter
         */
        void init(void *init_data);
        /*
         start the rpc adapter
         */
        void start();
        /*
         start the rpc adapter
         */
        void stop();
        /*
         deinit the rpc adapter
         */
        void deinit();
        
        //server worker thread
        /*!
         Thread where data is received and managed
         */
        void executeOnThread();
    };
    
}
#endif
