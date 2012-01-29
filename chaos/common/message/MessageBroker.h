    //
    //  MessageBroker.h
    //  CHAOSFramework
    //
    //  Created by Claudio Bisegni on 23/01/12.
    //  Copyright (c) 2012 INFN. All rights reserved.
    //

#ifndef ChaosFramework_MessageBroker_h
#define ChaosFramework_MessageBroker_h

#include <map>
#include <string>
#include <boost/thread/mutex.hpp>

#include <chaos/common/rpc/RpcClient.h>
#include <chaos/common/rpc/RpcServer.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/action/DeclareAction.h>
#include <chaos/common/dispatcher/CommandDispatcher.h>
#include <chaos/common/message/MessageChannel.h>
#include <chaos/common/utility/SetupStateManager.h>

namespace chaos {
    
    using namespace std;
    using namespace boost;
    
        //! Message Broker
    /*! 
     The MessageBroker is the manager for the message in chaos framework. It contains the reference to 
     chaos rpc client and server abstract class and to the message dispatcher abstract class. 
     It abstract the !CHAOS rule for sending message and wait for answer and other facility.
     */
    class MessageBroker: private SetupStateManager {
        
            //! Rpc client for message forwarding
        RpcClient *rpcClient;
        
            //! Rpc server for message listening
        RpcServer *rpcServer;
        
            //! Rpc server for message dispatcher
        CommandDispatcher *commandDispatcher;
        
            //!keep track of active channel
        map<string, MessageChannel*> activeChannel;
        
        string publishedHostAndPort;
        string metadataServerAddress;
        bool canUseMetadataServer;
        
    public:
            //! Channel Type Enumeration
        /*!
         Constants that identify the type of the channel to create
         */
        typedef enum {
            NORMAL = 0 /*!< Identify a raw channel used to send data pack to remote server */
        } EntityType;
        
            //! Basic Constructor
        MessageBroker();
        
            //! Basic Destructor
        ~MessageBroker();
        
            //!Message Broker initialization
        /*!
         * Initzialize the Message Broker. In this step are taken the configured implementation
         * for the rpc client and server and for the dispatcher. All these are here initialized
         */
        virtual void init() throw(CException);
        
            //!MessageBroker deinitialization
        /*!
         * All rpc adapter and command dispatcher are deinitilized. All instantiated channel are disposed
         */
        virtual void deinit() throw(CException);
        
            //!start all internal engine for client, server and message dispatcher
        /*!
         * all part are started
         */
        virtual void start() throw(CException);
        
            //! Action registration for the current isntance of MessageBroker
        /*!
         Register actions defined by AbstractActionDescriptor instance contained in the array
         */
        void registerAction(DeclareAction*);
        
            //!Action deregistration
        /*!
         Deregister actions owned by input parameter that are hosted in this current instance of message broker
         */
        void deregisterAction(DeclareAction*);
        
            //!Get the published port
        /*!
         Return the port where the rpc server has been published
         */
        int getPublishedPort();
        
            //!Get the serv:port description for the rpc server
        /*!
         Return the host and port where rpc server has benn published
         */
        void getPublishedHostAndPort(string&);
        
            //!message submition
        /*!
         Submit a message, all the inromation for forwarding it are already into CDataWrapper
         \param message the message coded into key/value semantics
         \param onThisThread if true the message is forwarded in the same thread of the caller
         */
        bool submitMessage(CDataWrapper *message, bool onThisThread=false);
        
            //!message submition
        /*!
         Submit a message specifing the destination
         \param serverAndPort server:port addres of remote endpoint 
         \param message the message coded into key/value semantics
         \param onThisThread if true the message is forwarded in the same thread of the caller
         */
        bool submitMessage(string& serveAndPort, CDataWrapper *message, bool onThisThread=false);
        
            //!message request
        /*!
         Submite a new request to send to the remote host
         \param serverAndPort server:port addres of remote endpoint 
         \param request the request coded into key/value semantics
         \param onThisThread if true the message is forwarded in the same thread of the caller
         */
        bool submiteRequest(string& serveAndPort,  CDataWrapper *request, bool onThisThread=false);
        
            //!message submition
        /*!
         Submit a message to the metadata server
         */
        bool submitMessageToMetadataServer(CDataWrapper*, bool onThisThread=false);
            
            //! Raw channel creation
        /*!
         Get new message channel for comunicate with remote host
         \param remoteHost host:port string htat identify the remote host 
         \param type channel type to create
         */
        MessageChannel *getNewMessageChannelForremoteHost(string& remoteHost, EntityType type);
    };
    
    
}


#endif
