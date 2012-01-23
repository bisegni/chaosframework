//
//  CommandDispatcher.h
//  ChaosFramework
//
//  Created by Claudio Bisegni on 01/05/11.
//  Copyright 2011 INFN. All rights reserved.
//

#ifndef CommandDispatcher_H
#define CommandDispatcher_H

#include <map>
#include <string>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

#include <chaos/common/action/DeclareAction.h>
#include <chaos/common/action/DomainActions.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/general/Configurable.h>

/*
 Base class for the command implementation
 */
namespace chaos{
    using namespace std;
    using namespace bson;
    using namespace boost;
    class RpcClient;
    
    //! Base class for the Chaos Action Dispatcher
    /*! 
     The base class implement all the default needs for the dispatcher. The sublcass need only
     to manage the priority execution all the registration of the domain and action are managed
     by this base class
     */
    class CommandDispatcher : public Configurable { 
        friend class RpcClient;
        string *typeName;
        
        //! Rpc Client for action result
        /*!Pointer to the associated rpc client, used to send the result of an action*/
        RpcClient *rpcClientPtr;
        
        //! Domain name <-> Action name association map
        /*!Contains the association between the domain name and all action for this domain*/
        map<string, shared_ptr<DomainActions> >  actionDomainExecutorMap;
        
    protected:

        //! Accessor to the object that manage the action for a domain
        /*!
         \return the instance of DomainActions pointer in relation to name
         but if the name is not present initialized it and add it to map
         */
        shared_ptr<DomainActions> getDomainActionsFromName(string&);
        
        //! Remove the infromation about a domain
        /*!
         \return an isntance of DomainActions pointer and remove
         it form the map
         */
        void  removeDomainActionsFromName(string&);

    public:
        //! Constructor
        CommandDispatcher(string *alias);
        
        
        //! Dispatch initialization with default value
        virtual void init(CDataWrapper*) throw(CException);
        
        
        //! Dispatch deinitialization with default value
        virtual void deinit() throw(CException);
        
        /*
         update the dispatcher configuration
         */
        virtual CDataWrapper* updateConfiguration(CDataWrapper*);
        
        //! Send a message via RPC with the associated client
        /*!
            \param messageToSend Is message wrapped into a CDataWrapper that needs to be forward via rpc channel
            \param serverAndPort Is the destination for the message. It has the form: "host:port"
            \param onThisThread Inform when the message need to be send on the current thread or scheduled 
             by rpc client own queue(if implemented). The default value is "false". In case of "false" value the
            deallocation is managed by rpc client, otherwise("true" value) the caller need to delete the object it self
            \return boolean value to informa is the mesage has been submitted
         */
        bool sendMessage(CDataWrapper* messageToSend, string& serverAndPort,  bool onThisThread = false);
        
        //! Send a message via RPC with the associated client
        /*!
         the information about destination need to be added in the message with the key:\n
         \n
         key = CommandManagerConstant::CS_CMDM_REMOTE_HOST_RESPONSE_ID\n
         value = id for the response\n
         \n
         key = CommandManagerConstant::CS_CMDM_REMOTE_HOST_IP\n
         value = the host:port string that repesent the endpoint
         
         \param messageToSend Is message wrapped into a CDataWrapper that needs to be forward via rpc channel
         \param onThisThread Inform when the message need to be send on the current thread or scheduled 
         by rpc client own queue(if implemented). The default value is "false". In case of "false" value the
         deallocation is managed by rpc client, otherwise("true" value) the caller need to delete the object it self
         \return boolean value to informa is the mesage has been submitted
         */
        bool  sendActionResult(CDataWrapper*, CDataWrapper*, bool onThisThread=false);
        
        //! dispatch the command accordin to protocol
        /*!
            Pure virtual function that need to be implemented by subclass for manage the priority or other thing befor
            the sending the message to the appropriate action
         */
        virtual CDataWrapper* dispatchCommand(CDataWrapper*)=0;
        
        //! Action registration
        /*
         This method provide the registration of the domain and action exposed by DeclareAction subclass. This method can be
         overrided by subclass for make some thing befor or after the registration
         \param declareActionClass The object that expose the domain and action name
         */
        virtual void registerAction(DeclareAction *declareActionClass);
        
        //! Action deregistration
        /*
         This method provide the deregistration of the domain and action exposed by DeclareAction subclass. This method can be
         overrided by subclass for make some thing befor or after the registration
         \param declareActionClass The object that expose the domain and action name
         */
        virtual void deregisterAction(DeclareAction *declareActionClass);
        
        /*
         Return the adapter alias
         */
        const char * getName() const;
        
        /*
         Set the rpc client
         */
        void setRpcClient(RpcClient*);

    };
}
#include "../rpc/RpcClient.h"

#endif