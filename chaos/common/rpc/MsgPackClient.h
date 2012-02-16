//
//  MsgPackClient.h
//  ChaosFramework
//
//  Created by Bisegni Claudio on 09/08/11.
//  Copyright (c) 2011 INFN. All rights reserved.
//

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

namespace chaos {
    
    /*
     Class that manage the MessagePack message send.
     */
     REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY(MsgPackClient, RpcClient), public CObjectProcessingQueue<CDataWrapper> {
            //messagepack connection pooling
        msgpack::rpc::session_pool *connectionPolling;
    protected:
        virtual void processBufferElement(CDataWrapper*, ElementManagingPolicy&) throw(CException);
        
    public:
         MsgPackClient(string *alias);
         ~MsgPackClient();
        /*
         init the rpc adapter
         */
        virtual void init(CDataWrapper*) throw(CException);
        
        /*
         start the rpc adapter
         */
        virtual void start() throw(CException);
        
        /*
         deinit the rpc adapter
         */
        virtual void deinit() throw(CException);
        
        /*
         Submit the message to be send to a certain ip, the datawrapper must contains
         the key CS_CMDM_REMOTE_HOST_IP
         */
        virtual bool submitMessage(CDataWrapper*, bool onThisThread=false) throw(CException);
    };
}
#endif
