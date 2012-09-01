/*
 *	EventTypeScheduler.h
 *	CHAOSFramework
 *	Created by Claudio Bisegni on 01/09/12.
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

#ifndef __CHAOSFramework__DomainEventScheduler__
#define __CHAOSFramework__DomainEventScheduler__

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
    
        //!Scheduler for action in a domain
    /*!
     This class define an environment where an aciotn can be executed
     */
    class EventTypeScheduler: private CObjectProcessingPriorityQueue<event::EventDescriptor> {
        friend class DefaultEventDispatcher;
            //! indicate the armed stato of this scheduler
        bool armed;
            //! mutext for regulate action execution and other operation as
            //! registration and deregistration
        boost::mutex eventSchedulerMutext;
            //!pointer to the domain containing action
        map<string, EventAction*> eventActionList;
    protected:
        void processBufferElement(event::EventDescriptor*, ElementManagingPolicy&) throw(CException);
        
        bool push(event::EventDescriptor *event) throw(CException);
    public:
        /*!
         Default constructor
         */
        EventTypeScheduler();
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
