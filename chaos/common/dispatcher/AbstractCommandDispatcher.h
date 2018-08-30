/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
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

#include <chaos/common/utility/NamedService.h>
#include <chaos/common/utility/StartableService.h>

/*
 Base class for the command implementation
 */
namespace chaos{
    namespace common {
        namespace network {
            class NetworkBroker;
        }
    }
    
    //forward decalration
    class AbstractCommandDispatcher;
    
    //! class for the echo test
    class EchoRpcAction:
    public DeclareAction {
    public:
        EchoRpcAction();
    protected:
        chaos::common::data::CDWUniquePtr echoAction(chaos::common::data::CDWUniquePtr action_data);
    };
    
    //! class for the check if an RPC domain is alive
    class CheckDomainRpcAction:
    public DeclareAction {
        AbstractCommandDispatcher *dispatcher;
    public:
        CheckDomainRpcAction(AbstractCommandDispatcher *_dispatcher);
    protected:
        chaos::common::data::CDWUniquePtr checkDomain(chaos::common::data::CDWUniquePtr action_data);
    };
    
    //! Base class for the Chaos Action Dispatcher
    /*!
     The base class implement all the default needs for the dispatcher. The sublcass need only
     to manage the priority execution all the registration of the domain and action are managed
     by this base class
     */
    class AbstractCommandDispatcher :
    public RpcServerHandler,
    public Configurable,
    public common::utility::StartableService,
    public common::utility::NamedService {
        friend class chaos::common::network::NetworkBroker;
        
        //! Rpc Client for action result
        /*!Pointer to the associated rpc client, used to send the result of an action*/
        RpcMessageForwarder *rpc_forwarder_ptr;
        
    protected:
        //! echo test class
        EchoRpcAction echo_action_class;
        //! check domain class
        CheckDomainRpcAction check_domain_action;
        
    public:
        //! Constructor
        AbstractCommandDispatcher(const std::string& alias);
        
        ~AbstractCommandDispatcher();
        
        virtual void init(void *init_data)  throw(CException);
        
        //-----------------------
        virtual void start() throw(CException);
        
        //-----------------------
        virtual void stop() throw(CException);
        
        virtual void deinit()  throw(CException);
        
        /*
         update the dispatcher configuration
         */
        virtual chaos::common::data::CDataWrapper* updateConfiguration(chaos::common::data::CDataWrapper*)  throw(CException);
        
        //! Send a message via RPC with the associated client
        /*! Send the message via rpc to a determinated node by ip and port.  If the message is a request, the pack need to
         contain the folowing key:\n
         \n
         key = CommandManagerConstant::CS_CMDM_REMOTE_HOST_RESPONSE_ID\n
         value = id for the response\n
         
         \param server_port Is the destination for the message. It has the form: "host:port"
         \param message Is message wrapped into a CDataWrapper that needs to be forward via rpc channel
         \param onThisThread Inform when the message need to be send on the current thread or scheduled
         by rpc client own queue(if implemented). The default value is "false". In case of "false" value the
         deallocation is managed by rpc client, otherwise("true" value) the caller need to delete the object it self
         \return boolean value to informa is the mesage has been submitted
         */
        bool submitMessage(const string& server_port,
                           chaos::common::data::CDWUniquePtr message,
                           bool onThisThread = false) throw(CException);
        
        //! Action registration
        /*
         This method provide the registration of the domain and action exposed by DeclareAction subclass. This method can be
         overrided by subclass for make some thing befor or after the registration
         \param declareActionClass The object that expose the domain and action name
         */
        virtual void registerAction(DeclareAction *declareActionClass)  throw(CException) = 0;
        
        //! Action deregistration
        /*
         This method provide the deregistration of the domain and action exposed by DeclareAction subclass. This method can be
         overrided by subclass for make some thing befor or after the registration
         \param declareActionClass The object that expose the domain and action name
         */
        virtual void deregisterAction(DeclareAction *declareActionClass)  throw(CException) = 0;
        
        virtual bool hasDomain(const std::string& domain_name) = 0;
        
        /*!
         Set the rpc forwarder implementation
         \param newRpcForwarderPtr rpc forwarder implementation instance
         */
        void setRpcForwarder(RpcMessageForwarder *newRpcForwarderPtr);
        
    };
}
#include <chaos/common/rpc/RpcClient.h>
#endif
