//
//  DefaultCommandDispatcher.h
//  ControlSystemLib
//
//  Created by Claudio Bisegni on 06/03/11.
//  Copyright 2011 INFN. All rights reserved.
//
#ifndef DefaultCommandDispatcher_H
#define DefaultCommandDispatcher_H

#include <string>
#include <boost/shared_ptr.hpp>
#include "DomainActionsScheduler.h"
#include <chaos/common/utility/ObjectFactoryRegister.h>

namespace chaos{
    
    using namespace std;
    using namespace boost;
    /*
     This class implemente the default CSLib dispatcher. It define an axecution buffer for every action domain
     */
    REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY(DefaultCommandDispatcher, CommandDispatcher) {
        bool deinitialized;
        
        map<string, shared_ptr<DomainActionsScheduler> > dasMap;
        
        /*
         return the scheduler for domain and if no present create a new instance
         */
        shared_ptr<DomainActionsScheduler>& getSchedulerForDomainName(string&);

    public:
        DefaultCommandDispatcher(string *alias):CommandDispatcher(alias){};
        /*
         Register actions defined by AbstractActionDescriptor instance contained in the array
         */
        virtual void registerAction(DeclareAction*);
        
        /*
         Deregister actions for a determianted domain
         */
        virtual void deregisterAction(DeclareAction*);
        
        /*
         This method sub the pack received by RPC system to the execution queue accordint to the pack domain
         the multithreading push is managed by OBuffer that is the superclass of DomainActionsScheduler. This method
         will ever return an allocated object. The deallocaiton is demanded to caller
         */
        virtual CDataWrapper* dispatchCommand(CDataWrapper*) throw(CException) ;
        /*
         Initialization method for output buffer
         */
        void init(CDataWrapper *initConfiguration) throw(CException);
        
        /*
         Deinitialization method for output buffer
         */
        void deinit() throw(CException);
    };
}
#endif