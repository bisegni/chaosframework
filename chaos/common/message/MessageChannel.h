//
//  MessageChannel.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 23/01/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#ifndef ChaosFramework_MessageChannel_h
#define ChaosFramework_MessageChannel_h

#include <chaos/common/utility/Atomic.h>
#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/action/DeclareAction.h>
#include <chaos/common/exception/CException.h>

#include <boost/function.hpp>
#include <boost/thread/condition.hpp>
#include <map>

namespace chaos {
    class MessageBroker;
    
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
        atomic_int_type channelRequestIDCounter;
        
        //! remote host where send the message and request
        string remoteHost;
        
        //! channel id (action domain)
        string channelID;
        
        boost::shared_mutex waith_asnwer_mutex;
        boost::condition    waith_asnwer_condition;
        typedef boost::shared_lock<boost::shared_mutex> lock;
        
        map<atomic_int_type, boost::function<void(CDataWrapper*)> > responsIdHandlerMap;
        map<atomic_int_type, CDataWrapper* > responseIdSyncMap;
        
        /*!
         Private constructor called by broker
         */
        MessageChannel(MessageBroker*, string&);
        
        /*!
         Private destructor called by broker
         */
        ~MessageChannel();
        
        /*!
         Initialization phase of the channe
         */
        virtual void init() throw(CException);

        /*!
         Initialization phase of the channe
         */
        virtual void deinit() throw(CException);
        
        /*! 
         \brief called when a response to a request is received, it will manage thesearch of
         hanlder specified for request id request
         */
        CDataWrapper* response(CDataWrapper*);
        
        /*!
         Set the reqeust id into the CDataWrapper
         */
        void prepareReqeustPack(CDataWrapper*, boost::function<void(CDataWrapper*)>&);
        /*!
         Set the reqeust id into the CDataWrapper
         \param requestPack the request pack to send
         \return the unique request id
         */
        atomic_int_type prepareReqeustPack(CDataWrapper*);
    public:
        
        /*! 
         \brief send a message
         */
        void sendMessage(const char * const, const char * const, CDataWrapper*);
        
        /*! 
         \brief send a request
         \param nodeID id of the node into remote chaos rpc system
         \param requestPack the data to send
         \param handler the callback that need to be called when the answer arrive
         */
        void sendRequest(const char * const, const char * const, CDataWrapper *, boost::function<void(CDataWrapper*)>);
        
        /*! 
         \brief send a request syncronous request
         \param nodeID id of the node into remote chaos rpc system
         \param actionName name of the actio to call
         \param requestPack the data to send
         \return the answer of the request
         */
        CDataWrapper* sendRequest(const char * const, const char * const, CDataWrapper *);
        
    };
    
}
#include <chaos/common/message/MessageBroker.h>
#endif
