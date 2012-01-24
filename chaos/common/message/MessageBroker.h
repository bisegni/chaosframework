//
//  MessageBroker.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 23/01/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#ifndef ChaosFramework_MessageBroker_h
#define ChaosFramework_MessageBroker_h

#include <boost/thread/recursive_mutex.hpp>

#include <chaos/common/rpc/RpcClient.h>
#include <chaos/common/rpc/RpcServer.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/action/DeclareAction.h>
#include <chaos/common/dispatcher/CommandDispatcher.h>
#include <chaos/common/message/MessageChannel.h>

#include <string>
#include <map>
namespace chaos {
    
    using namespace std;
    using namespace boost;
    
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
        
        //!keep track of active channel
        map<string, MessageChannel*> activeChannel;
        
        //!mutex for multithreading managment of sand box
        /*!
         The muthex is needed because the call to the action can occours in different thread
         */
        recursive_mutex managing_init_deinit_mutex;
        
        //! flag to set the brocer has initialized
        bool initialized;
        
        string publishedHostAndPort;
        string metadataServerAddress;
        bool canUseMetadataServer;
           
    public:
        /*!
         Define the constant used to identity the type of the channel to create
         */
        typedef enum {
            //! Normal channel is a raw channel used to send data pack to remote server
            NORMAL = 0,
        } EntityType;
        
        //! Basic Constructor
        MessageBroker();
        
        //! Basic Destructor
        ~MessageBroker();
        
        /*!
         * Initzialize the Message Broker. In this step are taken the configured implementation
         * for the rpc client and server and for the dispatcher. All these are here initialized
         */
        virtual void init() throw(CException);
        
        /*!
         * All rpc adapter and command siaptcer are deinitilized
         */
        virtual void deinit() throw(CException);
        
        /*!
         * all part are started
         */
        virtual void start() throw(CException);
                
        /*!
         Register actions defined by AbstractActionDescriptor instance contained in the array
         */
        void registerAction(DeclareAction*);
        
        /*!
         Deregister actions for a determianted domain
         */
        void deregisterAction(DeclareAction*);
        
        /*!
         Return the port where the rpc server has been published
         */
        int getPublishedPort();
        
        /*!
         Return the host and port for the MEssageBroker
         */
        void getPublishedHostAndPort(string&);
        
        /*!
         Submit a message and information for the destination servers are already setupped into CDataWrapper
         */
        bool submitMessage(CDataWrapper*, bool onThisThread=false);
        
        /*!
         Submit a message specifing the destination
         */
        bool submitMessage(string&, CDataWrapper*, bool onThisThread=false);
        
        /*!
         Submite a new request to send to the remote host
         */
        bool submiteRequest(string&,  CDataWrapper*, bool onThisThread=false);
        /*!
         Submit a message to the metadata server
         */
        bool submitMessageToMetadataServer(CDataWrapper*, bool onThisThread=false);

        /*!
         Get new message channel for comunicate with remote host
         \param remoteHost host:port string htat identify the remote host 
         \param type channel type to create
         */
        MessageChannel *getNewMessageChannelForremoteHost(string& remoteHost, EntityType type);
    };
    
    
}


#endif
