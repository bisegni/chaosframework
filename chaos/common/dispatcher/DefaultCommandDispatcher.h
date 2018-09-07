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

#ifndef DefaultCommandDispatcher_H
#define DefaultCommandDispatcher_H
//#pragma GCC diagnostic ignored "-Woverloaded-virtual"

#include <string>
#include <boost/shared_ptr.hpp>

#include <chaos/common/thread/ChaosThread.h>
#include <chaos/common/dispatcher/AbstractCommandDispatcher.h>
#include <chaos/common/dispatcher/DomainActionsScheduler.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>

namespace chaos{
    
    using namespace std;
    using namespace boost;
    /*!
     This class implemente the default CSLib dispatcher. It define an axecution buffer for every action domain
     */
    DECLARE_CLASS_FACTORY(DefaultCommandDispatcher, AbstractCommandDispatcher) {
        bool deinitialized;
        
		chaos::common::thread::SharedMutex das_map_mutex;
        map<string, ChaosSharedPtr<DomainActionsScheduler> > das_map;
    
        /*!
         return the scheduler for domain and if no present create a new instance
         */
        ChaosSharedPtr<DomainActionsScheduler>& getSchedulerForDomainName(string&);
        
        
        //! Domain name <-> Action name association map
        /*!Contains the association between the domain name and all action for this domain*/
        map<string, ChaosSharedPtr<DomainActions> >  action_domain_executor_map;
        
        //! Accessor to the object that manage the action for a domain
        /*!
         \return the instance of DomainActions pointer in relation to name
         but if the name is not present initialized it and add it to map
         */
        ChaosSharedPtr<DomainActions> getDomainActionsFromName(const string& domain_name);
        
        //! Remove the infromation about a domain
        /*!
         \return an isntance of DomainActions pointer and remove
         it form the map
         */
        void  removeDomainActionsFromName(const string& domain_name);
        
    public:
        DefaultCommandDispatcher(string alias);
        virtual ~DefaultCommandDispatcher();
        /*!
         Initialization method for output buffer
         */
        void init(void *) throw(CException);
        
        /*!
         Deinitialization method for output buffer
         */
        void deinit() throw(CException);
        /*!
         Register actions defined by AbstractActionDescriptor instance contained in the array
         */
        virtual void registerAction(DeclareAction*)  throw(CException) ;
        
        /*!
         Deregister actions for a determianted domain
         */
        virtual void deregisterAction(DeclareAction*)  throw(CException) ;
        
		// inherited method
        chaos::common::data::CDWUniquePtr dispatchCommand(chaos::common::data::CDWUniquePtr message_data);
		
		// inherited method
		chaos::common::data::CDWUniquePtr executeCommandSync(chaos::common::data::CDWUniquePtr message_data);
        
        //inherited method
        uint32_t domainRPCActionQueued(const std::string& domain_name);
        
        bool hasDomain(const std::string& domain_name);
    };
}
#endif
