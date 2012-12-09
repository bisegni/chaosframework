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
#include <chaos/common/action/EventAction.h>
#include <chaos/common/dispatcher/EventTypeScheduler.h>

#define DESLAPP_ LAPP_ << "[DomainEventScheduler] - "

using namespace chaos;

EventTypeScheduler::EventTypeScheduler():armed(false){
}

EventTypeScheduler::~EventTypeScheduler() {
    
}

/*
 Initialization method for output buffer
 */
void EventTypeScheduler::init(int threadNumber) throw(CException) {
    DESLAPP_ << "Initializing";
    mutex::scoped_lock lockAction(eventSchedulerMutext);
    CObjectProcessingPriorityQueue<event::EventDescriptor>::init(threadNumber);
    armed = true;
}

/*
 Deinitialization method for output buffer
 */
void EventTypeScheduler::deinit() throw(CException) {
    DESLAPP_ << "Deinitializing ";
    mutex::scoped_lock lockAction(eventSchedulerMutext);
    armed = false;
    lockAction.unlock();
    CObjectProcessingPriorityQueue<event::EventDescriptor>::clear();
    CObjectProcessingPriorityQueue<event::EventDescriptor>::deinit();

}

/*
 override the push method for ObjectProcessingQueue<CDataWrapper> superclass
 */
bool EventTypeScheduler::push(event::EventDescriptor *event) throw(CException) {
    mutex::scoped_lock lockAction(eventSchedulerMutext);
    if(!armed) throw CException(0, "Event can't be processed, scheduler is not armed", "EventTypeScheduler::push");
    return CObjectProcessingPriorityQueue<event::EventDescriptor>::push(event, event->getEventPriority());
}

void EventTypeScheduler::installEventAction(EventAction *eventAction) {
    mutex::scoped_lock lockAction(eventSchedulerMutext);
    if(eventActionList.count(eventAction->getUUID()) > 0) return;
        //add action
    eventActionList.insert(make_pair(eventAction->getUUID(), eventAction));
    
    eventAction->setEnabled(true);
}

void EventTypeScheduler::removeEventAction(EventAction *eventAction) {
    if(!eventAction) return;
    mutex::scoped_lock lockAction(eventSchedulerMutext);
    if(eventActionList.count(eventAction->getUUID()) == 0) return;
    
        //shutdown the state for fire the action
    eventAction->setEnabled(false);
    
        //add action
    eventActionList.erase(eventAction->getUUID());
}

/*
 process the element action to be executed
 */
void EventTypeScheduler::processBufferElement(event::EventDescriptor *eventDescription, ElementManagingPolicy& elementPolicy) throw(CException) {
    mutex::scoped_lock lockAction(eventSchedulerMutext);
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
    
}
