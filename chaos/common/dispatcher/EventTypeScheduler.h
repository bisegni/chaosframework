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

#ifndef __CHAOSFramework__DomainEventScheduler__
#define __CHAOSFramework__DomainEventScheduler__
#pragma GCC diagnostic ignored "-Woverloaded-virtual"

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <map>
#include <chaos/common/action/DomainActions.h>
#include <chaos/common/dispatcher/AbstractCommandDispatcher.h>
#include <chaos/common/pqueue/ChaosProcessingQueue.h>
#include <chaos/common/event/evt_desc/EventDescriptor.h>

namespace chaos {
    using namespace std;
    using namespace boost;
    
        //Forward declaration
    class EventAction;
    class DefaultEventDispatcher;
    
        //!Scheduler for event action
    /*!
     This class define an environment where an action for an event can be executed
     */
    class EventTypeScheduler:
    private chaos::common::pqueue::CObjectProcessingPriorityQueue<common::event::EventDescriptor> {
        friend class DefaultEventDispatcher;
            //! indicate the armed stato of this scheduler
        bool armed;
            //! mutext for regulate action execution and other operation as
            //! registration and deregistration
        boost::mutex eventSchedulerMutext;
            //!pointer to the domain containing action
        map<string, EventAction*> eventActionList;
    protected:
        void processBufferElement(common::event::EventDescriptor*, ElementManagingPolicy&) throw(CException);
        
        bool push(common::event::EventDescriptor *event) throw(CException);
    public:
        /*!
         Default constructor
         */
        EventTypeScheduler();
        
        /*!
         Default destructor
         */
        virtual ~EventTypeScheduler();
        
        /*!
         Return the domain name managed by the sceduler instance
         */
        string& getManagedDomainName();
        
        /*!
         Initialization method
         */
        void init(int threadNumber = 1) throw(CException);
        
        /*!
         Deinitialization method
         */
        void deinit() throw(CException);
        
        void installEventAction(EventAction*);
        
        void removeEventAction(EventAction*);

    };
}

#endif /* defined(__CHAOSFramework__DomainEventScheduler__) */
