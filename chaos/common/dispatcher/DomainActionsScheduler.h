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

#ifndef ChaosFramework_ActionsScheduler_h
#define ChaosFramework_ActionsScheduler_h
//#pragma GCC diagnostic ignored "-Woverloaded-virtual"

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include <chaos/common/action/DomainActions.h>
#include <chaos/common/dispatcher/AbstractCommandDispatcher.h>
#include <chaos/common/pqueue/ChaosProcessingQueue.h>
#include <chaos/common/data/CDataWrapper.h>

namespace chaos_data = chaos::common::data;

namespace chaos {
    using namespace std;
    using namespace boost;
        //!Scheduler for action in a domain
    /*!
     This class define an environment where an aciotn can be executed
     */
    class DomainActionsScheduler:
    private CObjectProcessingQueue<chaos::common::data::CDataWrapper> {
        friend class CommandDispatcher;
            //! indicate the armed stato of this scheduler
        bool armed;
            //! mutext for regulate action execution and other operation as
            //! registration and deregistration
        boost::mutex actionAccessMutext;
            //!pointer to the domain containing action
        ChaosSharedPtr<DomainActions> domainActionsContainer;
        
            //!reference to global dispatcher used to resubmit sub command
        AbstractCommandDispatcher *dispatcher;
    public:
        virtual void processBufferElement(chaos::common::data::CDWShrdPtr element) throw(CException);
		
		//!call the action in an async way
        virtual void synchronousCall(const std::string& action,
                                     chaos::common::data::CDWUniquePtr message,
                                     chaos::common::data::CDWUniquePtr& result);
    public:
        /*!
         Default constructor
         */
        DomainActionsScheduler(ChaosSharedPtr<DomainActions>);
        /*!
         Default destructor
         */
        virtual ~DomainActionsScheduler();
        /*!
            Return the domain name managed by the sceduler instance
         */
        const string& getManagedDomainName();
        
        /*!
         Initialization method
         */
        virtual void init(int) throw(CException);
        
        /*!
         Deinitialization method
         */
        virtual void deinit() throw(CException);
        /*!
         Push a new action pack into the queue
         */
        bool push(chaos::common::data::CDWUniquePtr rpc_action_call) throw(CException);
        /*!
         Set the current dispatcher
         */
        void setDispatcher(AbstractCommandDispatcher*);
        
        //! return the number of queued action
        uint32_t getQueuedActionSize();
    };
}
#endif
