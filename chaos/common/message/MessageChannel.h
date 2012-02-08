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
#include <chaos/common/thread/MultiKeyObjectWaitSemaphore.h>
#include <chaos/common/rpcnet/CNodeNetworkAddress.h>

#include <boost/function.hpp>
#include <boost/thread/condition.hpp>
#include <map>

namespace chaos {
    
    class MessageBroker;
    
        //! MessageChannel
    /*! 
     This is the basic channel for comunicate with other chaos rpc server. It can be instantiated only by
     MessageBroker. It can send a message or a request. Message is the async forward data to domain/action hosted on remote rpc server and no answer is waited.
     Request is two pahse message, first a message(the reqeust) is sent to  domain/action on remote rpc server, then an aswer is waited this step can
     be achieved in two way:
     1 - Async mode, an handler must be given on reqeust call
     2 - Sync mode, wait until the answer is received, or can be set a number of milliseconds to wait for.
     */
    class MessageChannel : public DeclareAction {
        friend class MessageBroker;
            //! Message broker associated with the channel instance
        MessageBroker *broker;
        
            //! atomic int for request id
        atomic_int_type channelRequestIDCounter;
        
            //! remote host where send the message and request
        string remoteNodeAddress;
        
            //! channel id (action domain)
        string channelID;
        
            //!multi key semaphore for manage the return of the action and result association to the reqeust id
        MultiKeyObjectWaitSemaphore<atomic_int_type,CDataWrapper*> sem;
        
            //!map to async request and handler
        map<atomic_int_type, boost::function<void(CDataWrapper*)> > responsIdHandlerMap;
        
            //!map to sync request and result
        map<atomic_int_type, CDataWrapper* > responseIdSyncMap;
        
        /*!
         Initialization phase of the channel
         */
        virtual void init() throw(CException);
        
        /*!
         Initialization phase of the channel
         */
        virtual void deinit() throw(CException);
        
        /*! 
         \brief called when a response to a request is received, it will manage thesearch of
         hanlder specified for request id request
         */
        CDataWrapper* response(CDataWrapper*, bool&);
        
        /*!
         Set the reqeust id into the CDataWrapper
         \param requestPack the request pack to send
         \return the unique request id
         */
        atomic_int_type prepareRequestPackAndSend(const char * const, const char * const, CDataWrapper*, boost::function<void(CDataWrapper*)>*);
   
    protected:
        /*!
         Return the broker for that channel
         */
        MessageBroker * getBroker(){
            return broker;
        }
        
        /*!
         Private constructor called by MessageBroker
         */
        MessageChannel(MessageBroker*, const char*const);
        
        /*!
         Private constructor called by MessageBroker
         */
        MessageChannel(MessageBroker*);
        
        /*!
         Private destructor called by MessageBroker
         */
        ~MessageChannel();

        /*!
         Update the address of the channel
         */
        void setRemoteNodeAddress(string& remoteAddr){
            remoteNodeAddress = remoteAddr;
        }
    public:

        /*! 
         \brief send a message
         \param messagePack the data to send, the pointer is not deallocated and i scopied into the pack
         */
        void sendMessage(const char * const, const char * const, CDataWrapper* const);
        
        /*! 
         \brief send a request
         \param nodeID id of the node into remote chaos rpc system
         \param requestPack the data to send, the pointer is not deallocated and i scopied into the pack
         \param handler the callback that need to be called when the answer arrive
         */
        void sendRequest(const char * const, const char * const, CDataWrapper* const, boost::function<void(CDataWrapper*)>);
        
        /*! 
         \brief send a syncronous request and can wait for a determinated number of milliseconds the answer. If it has not
         been received the method return with a NULL pointer
         \param nodeID id of the node into remote chaos rpc system
         \param actionName name of the actio to call
         \param requestPack the data to send, the pointer is not deallocated and i scopied into the pack
         \param millisecToWait waith the response for onli these number of millisec then return
         \return the answer of the request, a null value mean that the wait time is expired 
         */
        CDataWrapper* sendRequest(const char * const, const char * const, CDataWrapper* const, uint32_t millisecToWait=0);
        
    };
    
}
#endif
