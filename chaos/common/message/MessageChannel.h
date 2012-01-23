//
//  MessageChannel.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 23/01/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#ifndef ChaosFramework_MessageChannel_h
#define ChaosFramework_MessageChannel_h

#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/action/DeclareAction.h>
#include <chaos/common/message/MessageBroker.h>
#include <chaos/common/utility/Atomic.h>
#include <boost/function.hpp>

namespace chaos {
    
    //! MessageChannel
    /*! 
     This is the channel for talking with determinated server, it will mamange all necessary 
     action to simulate sincronous or asincronous operation. It can be instantiated only by
     MessageBroker. It work in two mode:
     1 - can send message
     2 - can send request and waith for the answer
     */
    class MessageChannel : public DeclareAction {
        friend class MessageBroker;
        MessageBroker *broker;
        
        //! atomic int for request id
        atomic_int_type channelRequestID;
        
        //! remote host where send the message and request
        string remoteHost;
        
        //! channel id (action domain)
        string localChannelID;
        
        /*!
         Private constructor called by broker
         */
        MessageChannel(MessageBroker*, string&);
        
        /*!
         Private destructor called by broker
         */
        ~MessageChannel();
        
        /*! 
         \brief called when a response to a request is received, it will manage thesearch of
         hanlder specified for request id request
         */
        void response(CDataWrapper*);
        
    public:
        
        /*! 
         \brief send a message
         */
        void sendMessage(CDataWrapper*);
        
        /*! 
         \brief send a request
         \param requestMessage the data to send
         \param handler the callback that need to be called when the answer arrive
         */
        void sendRequest(CDataWrapper *, boost::function<void(CDataWrapper*)>);
        
    };
    
}

#endif
