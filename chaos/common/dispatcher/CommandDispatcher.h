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
    
        //class RpcAdapter;
    class CommandDispatcher : public Configurable { 
        friend class RpcClient;
        string *typeName;
        
        /*
         This is the rpivate instace use from CommandDispatcher sublcass
         to send the answer for an action
         */
        shared_ptr<RpcClient> rpcClientPtr;
        
        //map containing the hash for the action
        map<string, shared_ptr<DomainActions> >  actionDomainExecutorMap;
        
    protected:

        
        /*
         return an isntance of DomainActions pointer in relation to name
         but if the name is not present initialized it and add it to map
         */
        shared_ptr<DomainActions> getDomainActionsFromName(string&);
        
        /*
         return an isntance of DomainActions pointer and remove
         it form the map
         */
        void  removeDomainActionsFromName(string&);


        
    public:
        CommandDispatcher(string *alias);
        
        /*
         Send a message to a determinated IP
         */
        bool sendMessage(CDataWrapper*, string&,  bool onThisThread=false);
        
        /*
         Submit the action answer to rpc system, it return true if
         the result has been sent, otherwise false. IN case of
         true result the memory  associated to the second parameter
         is managed by the system
         */
        bool  sendActionResult(CDataWrapper*, CDataWrapper*, bool onThisThread=false);
        
        /*
         dispatch the command accordin to protocol
         */
        virtual CDataWrapper* dispatchCommand(CDataWrapper*)=0;
        
        /*
         Register actions defined by AbstractActionDescriptor instance contained in the array
         */
        virtual void registerAction(DeclareAction*);
        
        /*
         Deregister actions for a determianted domain
         */
        virtual void deregisterAction(DeclareAction*);
        
        /*
         init the rpc adapter
         */
        virtual void init(CDataWrapper*) throw(CException);
        
        
        /*
         deinit the rpc adapter
         */
        virtual void deinit() throw(CException);
        
        /*
         update the dispatcher configuration
         */
        virtual CDataWrapper* updateConfiguration(CDataWrapper*);
        
        /*
         Return the adapter alias
         */
        const char * getName() const;
        
        /*
         Set the rpc client
         */
        void setRpcClient(shared_ptr<RpcClient>);

    };
}
#include "../rpc/RpcClient.h"

#endif