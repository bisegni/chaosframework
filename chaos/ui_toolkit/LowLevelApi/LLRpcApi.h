    //
    //  LLRpcApi.h
    //  ChaosFramework
    //
    //  Created by Bisegni Claudio on 30/10/11.
    //  Copyright (c) 2011 INFN. All rights reserved.
    //

#ifndef ChaosFramework_LLRpcApi_h
#define ChaosFramework_LLRpcApi_h


#include <boost/shared_ptr.hpp>

#include <chaos/common/utility/Singleton.h>
#include <chaos/common/exception/CException.h>
#include <chaos/common/message/MessageBroker.h>
#include <chaos/common/message/MessageChannel.h>
namespace chaos {
    namespace ui{
        using namespace boost;  
        
        /*
         LLRpcApi Class api for rpc system
         */
        class LLRpcApi : public Singleton<LLRpcApi> {
            friend class ChaosUIToolkit;
            friend class Singleton<LLRpcApi>;
            
            chaos::MessageBroker *rpcMessageBroker;
            /*
             LL Rpc Api static initialization it should be called once for application
             */
            void init() throw (CException);
            /*
             Deinitialization of LL rpc api
             */
            void deinit() throw (CException);
        
            /*
             */
            LLRpcApi();
            
            /*
             */
            ~LLRpcApi();
            
        public:
            
            /*!
             Return a new channel for talk with metadata server
             */
            MessageChannel *getNewMetadataServerChannel();
        };
    }
}


#endif
