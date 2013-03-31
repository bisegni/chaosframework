//
//  ZMQClient.h
//  CHAOSFramework
//
//  Created by Bisegni Claudio on 11/03/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#ifndef CHAOSFramework_ZMQClient_h
#define CHAOSFramework_ZMQClient_h
#ifdef DEV_WITH_ZMQ

#pragma GCC diagnostic ignored "-Woverloaded-virtual"

#include <chaos/common/rpc/RpcClient.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/pqueue/ChaosProcessingQueue.h>
#include <chaos/common/exception/CException.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>

#include <zmq.h>

namespace chaos {
    
    /*
     Class that manage the MessagePack message send.
     */
    REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY(ZMQClient, RpcClient), public CObjectProcessingQueue<NetworkForwardInfo> {

    protected:
        virtual void processBufferElement(NetworkForwardInfo*, ElementManagingPolicy&) throw(CException);
        void *zmqContext;
    public:
        ZMQClient(string *alias);
        virtual ~ZMQClient();
        /*
         init the rpc adapter
         */
        void init(CDataWrapper*) throw(CException);
        
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
         Submit the message to be send to a certain ip, the datawrapper must contains
         the key CS_CMDM_REMOTE_HOST_IP
         */
        bool submitMessage(NetworkForwardInfo *forwardInfo, bool onThisThread=false) throw(CException);
    };
}
#endif
#endif
