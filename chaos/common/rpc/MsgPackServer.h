    //
    //  MsgPackServer.h
    //  ChaosFramework
    //
    //  Created by Claudio Bisegni on 01/05/11.
    //  Copyright 2011 INFN. All rights reserved.
    //
#ifndef MsgPackServer_H
#define MsgPackServer_H

#include "RpcServer.h"
#include <boost/shared_ptr.hpp>
#include <msgpack/rpc/server.h>


#include <chaos/common/exception/CException.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>

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
        
        MsgPackServer(string *alias):RpcServer(alias){};
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
        
        /*
         
         */
        void dispatch(request req);
    };
}
#endif

