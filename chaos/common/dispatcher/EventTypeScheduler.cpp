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
#include <chaos/common/action/EventAction.h>
#include <chaos/common/dispatcher/EventTypeScheduler.h>

#define DESLAPP_ LAPP_ << "[DomainEventScheduler] - "

using namespace chaos;
using namespace boost;
using namespace chaos::common::event;
EventTypeScheduler::EventTypeScheduler():armed(false){
}

EventTypeScheduler::~EventTypeScheduler() {
    
}

/*
 Initialization method for output buffer
 */
void EventTypeScheduler::init(int threadNumber) throw(CException) {
    DESLAPP_ << "Initializing";
    boost::mutex::scoped_lock lockAction(eventSchedulerMutext);
    CObjectProcessingPriorityQueue<EventDescriptor>::init(threadNumber);
    armed = true;
}

/*
 Deinitialization method for output buffer
 */
void EventTypeScheduler::deinit() throw(CException) {
    DESLAPP_ << "Deinitializing ";
    boost::mutex::scoped_lock lockAction(eventSchedulerMutext);
    armed = false;
    lockAction.unlock();
    CObjectProcessingPriorityQueue<EventDescriptor>::clear();
    CObjectProcessingPriorityQueue<EventDescriptor>::deinit();

}

/*
 override the push method for ObjectProcessingQueue<CDataWrapper> superclass
 */
bool EventTypeScheduler::push(EventDescriptor *event) throw(CException) {
    boost::mutex::scoped_lock lockAction(eventSchedulerMutext);
    if(!armed) throw CException(0, "Event can't be processed, scheduler is not armed", "EventTypeScheduler::push");
    return CObjectProcessingPriorityQueue<EventDescriptor>::push(event, event->getEventPriority());
}

void EventTypeScheduler::installEventAction(EventAction *eventAction) {
    boost::mutex::scoped_lock lockAction(eventSchedulerMutext);
    if(eventActionList.count(eventAction->getUUID()) > 0) return;
        //add action
    eventActionList.insert(make_pair(eventAction->getUUID(), eventAction));
    
    eventAction->setEnabled(true);
}

void EventTypeScheduler::removeEventAction(EventAction *eventAction) {
    if(!eventAction) return;
    boost::mutex::scoped_lock lockAction(eventSchedulerMutext);
    if(eventActionList.count(eventAction->getUUID()) == 0) return;
    
        //shutdown the state for fire the action
    eventAction->setEnabled(false);
    
        //add action
    eventActionList.erase(eventAction->getUUID());
}

/*
 process the element action to be executed
 */
void EventTypeScheduler::processBufferElement(EventDescriptor *eventDescription, ElementManagingPolicy& elementPolicy) throw(CException) {
    boost::mutex::scoped_lock lockAction(eventSchedulerMutext);
    for ( map<string, EventAction*>::iterator iter =  eventActionList.begin();
         iter != eventActionList.end();
         iter++) {
        AESS_ReadLock readLockForActionExecution( (*iter).second->aessMutext);
       
        bool canbeExecuted = (*iter).second->setFired(true);
        if(!canbeExecuted) continue;
        
            //check if we can start it with the indetifier of the event
        string identifier(eventDescription->getIdentification(), eventDescription->getIdentificationlength());
        if((*iter).second->hasIdentifier(identifier.c_str())) {
                //execute the action
            (*iter).second->handleEvent(eventDescription);
        }
        (*iter).second->setFired(false);
    }
        //NOTE the eventDescription memory is managed directly by the queue because we don't modify the elementPolicy.elementHasBeenDetached property
}
