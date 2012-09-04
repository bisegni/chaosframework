/*	
 *	DefaultCommandDispatcher.h
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

#ifndef DefaultCommandDispatcher_H
#define DefaultCommandDispatcher_H

#include <string>
#include <boost/shared_ptr.hpp>
#include <chaos/common/dispatcher/AbstractCommandDispatcher.h>
#include <chaos/common/dispatcher/DomainActionsScheduler.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>

namespace chaos{
    
    using namespace std;
    using namespace boost;
    /*!
     This class implemente the default CSLib dispatcher. It define an axecution buffer for every action domain
     */
    REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY(DefaultCommandDispatcher, AbstractCommandDispatcher) {
        bool deinitialized;
        
        map<string, shared_ptr<DomainActionsScheduler> > dasMap;
    
        /*!
         return the scheduler for domain and if no present create a new instance
         */
        shared_ptr<DomainActionsScheduler>& getSchedulerForDomainName(string&);
        
    protected:
        /*!
         Initialization method for output buffer
         */
        void init(CDataWrapper *initConfiguration) throw(CException);
        
        /*!
         Deinitialization method for output buffer
         */
        void deinit() throw(CException);

    public:
        DefaultCommandDispatcher(string *alias) : AbstractCommandDispatcher(alias){};
        /*!
         Register actions defined by AbstractActionDescriptor instance contained in the array
         */
        virtual void registerAction(DeclareAction*)  throw(CException) ;
        
        /*!
         Deregister actions for a determianted domain
         */
        virtual void deregisterAction(DeclareAction*)  throw(CException) ;
        
        /*!
         This method sub the pack received by RPC system to the execution queue accordint to the pack domain
         the multithreading push is managed by OBuffer that is the superclass of DomainActionsScheduler. This method
         will ever return an allocated object. The deallocaiton is demanded to caller
         */
        virtual CDataWrapper* dispatchCommand(CDataWrapper*) throw(CException);

    };
}
#endif
