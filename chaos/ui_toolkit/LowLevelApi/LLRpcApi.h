    //
    //  LLRpcApi.h
    //  ChaosFramework
    //
    //  Created by Bisegni Claudio on 30/10/11.
    //  Copyright (c) 2011 INFN. All rights reserved.
    //

#ifndef ChaosFramework_LLRpcApi_h
#define ChaosFramework_LLRpcApi_h

#include <chaos/common/rpc/ChaosRpc.h>
#include <chaos/common/dispatcher/ChaosDispatcher.h>
#include <chaos/common/exception/CException.h>

#include <boost/shared_ptr.hpp>

namespace chaos {
    namespace ui{
        using namespace boost;        
        /*
         LLRpcApi Class api for rpc system
         */
        class LLRpcApi {
            friend class ChaosUIToolkit;
            
            static shared_ptr<RpcServer> rpcServer;
            static shared_ptr<RpcClient> rpcClient;
            static shared_ptr<CommandDispatcher> commandDispatcher;
            /*
             LL Rpc Api static initialization it should be called once for application
             */
            static void initRpcApi(CDataWrapper*) throw (CException);
            /*
             Deinitialization of LL rpc api
             */
            static void deinitRpcApi() throw (CException);
        public:
            LLRpcApi();
            ~LLRpcApi();
            
            
            
        };
    }
}

#endif
