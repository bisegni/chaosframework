/*
 *	AbstractCommandDispatcher.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
 */

#ifndef AbstractCommandDispatcher_H
#define AbstractCommandDispatcher_H

#include <map>
#include <string>
    //#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

#include <chaos/common/rpc/RpcMessageForwarder.h>
#include <chaos/common/rpc/RpcServerHandler.h>

#include <chaos/common/action/DeclareAction.h>
#include <chaos/common/action/DomainActions.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/general/Configurable.h>
#include <chaos/common/utility/ISDInterface.h>

/*
 Base class for the command implementation
 */
namespace chaos{
    using namespace std;
    using namespace bson;
    using namespace boost;
        //class RpcClient;
    
    class NetworkBroker;
    
        //! Base class for the Chaos Action Dispatcher
    /*!
     The base class implement all the default needs for the dispatcher. The sublcass need only
     to manage the priority execution all the registration of the domain and action are managed
     by this base class
     */
    class AbstractCommandDispatcher : public RpcServerHandler, Configurable, utility::ISDInterface {
        friend class NetworkBroker;
            //friend class RpcClient;
        string *typeName;
        
            //! Rpc Client for action result
        /*!Pointer to the associated rpc client, used to send the result of an action*/
        RpcMessageForwarder *rpcForwarderPtr;
        
            //! Domain name <-> Action name association map
        /*!Contains the association between the domain name and all action for this domain*/
        map<string, shared_ptr<DomainActions> >  actionDomainExecutorMap;
        
    protected:
        
            //! Dispatch initialization with default value
        virtual void init(CDataWrapper*) throw(CException);
        
            //-----------------------
        virtual void start() throw(CException);
        
            //! Dispatch deinitialization with default value
        virtual void deinit() throw(CException);
        
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
        AbstractCommandDispatcher(string *alias);
        
        /*
         update the dispatcher configuration
         */
        virtual CDataWrapper* updateConfiguration(CDataWrapper*)  throw(CException);
        
            //! Send a message via RPC with the associated client
        /*! Send the message via rpc to a determinated node by ip and port.  If the message is a request, the pack need to
         contain the folowing key:\n
         \n
         key = CommandManagerConstant::CS_CMDM_REMOTE_HOST_RESPONSE_ID\n
         value = id for the response\n
         
         \param serverAndPort Is the destination for the message. It has the form: "host:port"
         \param messageToSend Is message wrapped into a CDataWrapper that needs to be forward via rpc channel
         \param onThisThread Inform when the message need to be send on the current thread or scheduled
         by rpc client own queue(if implemented). The default value is "false". In case of "false" value the
         deallocation is managed by rpc client, otherwise("true" value) the caller need to delete the object it self
         \return boolean value to informa is the mesage has been submitted
         */
        bool submitMessage(string& serverAndPort,  CDataWrapper* messageToSend, bool onThisThread = false)  throw(CException);
        
            //! Action registration
        /*
         This method provide the registration of the domain and action exposed by DeclareAction subclass. This method can be
         overrided by subclass for make some thing befor or after the registration
         \param declareActionClass The object that expose the domain and action name
         */
        virtual void registerAction(DeclareAction *declareActionClass)  throw(CException) ;
        
            //! Action deregistration
        /*
         This method provide the deregistration of the domain and action exposed by DeclareAction subclass. This method can be
         overrided by subclass for make some thing befor or after the registration
         \param declareActionClass The object that expose the domain and action name
         */
        virtual void deregisterAction(DeclareAction *declareActionClass)  throw(CException) ;
        
        /*!
         Ghet the name of tge dispatcher
         \return the adapter alias
         */
        const char * getName() const;
        
        /*!
         Set the rpc forwarder implementation
         \param newRpcForwarderPtr rpc forwarder implementation instance
         */
        void setRpcForwarder(RpcMessageForwarder *newRpcForwarderPtr);
        
    };
}
#include <chaos/common/rpc/RpcClient.h>
#endif
