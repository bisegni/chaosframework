//
//  MessageBroker.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 23/01/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#ifndef ChaosFramework_MessageBroker_h
#define ChaosFramework_MessageBroker_h

#include <chaos/common/rpc/RpcClient.h>
#include <chaos/common/rpc/RpcServer.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/action/DeclareAction.h>
#include <chaos/common/dispatcher/CommandDispatcher.h>

namespace chaos {
    
    //! Message Broker
    /*! 
     This class is the manager for the message procher in chaos framework. It contains the reference to 
     chaos rpc client and server abstract class and to the message dispatcher abstract class. 
     It abstract the !CHAOS rule for sending message and waith for answer and other facility as talk to metadata server
     */
    class MessageBroker {
        //! Rpc client for message forwarding
        RpcClient *rpcClient;
        
        //! Rpc server for message listening
        RpcServer *rpcServer;
        
        //! Rpc server for message dispatcher
        CommandDispatcher *commandDispatcher;
        string metadataServerAddress;
        bool canUseMetadataServer;
           
    public:
        
        MessageBroker();
        ~MessageBroker();
        
        /*!
         * Initzialize the Message Broker. In this step are taken the configured implementation
         * for the rpc client and server and for the dispatcher. All these are here initialized
         */
        virtual void init() throw(CException) = 0;
        
        /*!
         * All rpc adapter and command siaptcer are deinitilized
         */
        virtual void deinit() throw(CException) = 0;
        
        /*!
         * all part are started
         */
        virtual void start() throw(CException) = 0;
                
        /*!
         Register actions defined by AbstractActionDescriptor instance contained in the array
         */
        void registerAction(DeclareAction*);
        
        /*!
         Deregister actions for a determianted domain
         */
        void deregisterAction(DeclareAction*);
        
        /*!
         Submit a message and information for the destination servers are already setupped into CDataWrapper
         */
        bool submitMessage(CDataWrapper*, bool onThisThread=false);
        
        /*!
         Submit a message specifing the destination
         */
        bool submitMessage(string&, CDataWrapper*, bool onThisThread=false);
        
        /*!
         Submit a message to the metadata server
         */
        bool submitMessageToMetadataServer(CDataWrapper*, bool onThisThread=false);

    };
    
    
}


#endif
