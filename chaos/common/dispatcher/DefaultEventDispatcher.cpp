/*
 *	DefaultEventDispatcher.cpp
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


#include <chaos/common/dispatcher/DefaultEventDispatcher.h>
#include <chaos/common/dispatcher/EventTypeScheduler.h>

using namespace chaos;
using namespace chaos::event;

#define EVTDISPAPP_ LAPP_ << "[DefaultEventDispatcher] - "

    //! Basic Constructor
DefaultEventDispatcher::DefaultEventDispatcher(string *alias):AbstractEventDispatcher(alias) {
    
}

    //! Basic Destructor
DefaultEventDispatcher::~DefaultEventDispatcher() {
    
}

void DefaultEventDispatcher::init(CDataWrapper* initData) throw(CException) {
    EVTDISPAPP_ << "Init";
    alertEventScheduler = new EventTypeScheduler();
    if(!alertEventScheduler) throw CException(0, "Error allocating Alert Event Scheduler", "DefaultEventDispatcher::init");
    alertEventScheduler->init();
    
    
    instrumentEventScheduler = new EventTypeScheduler();
    if(!instrumentEventScheduler) throw CException(0, "Error allocating Instrument Event Scheduler", "DefaultEventDispatcher::init");
    instrumentEventScheduler->init();
}

void DefaultEventDispatcher::start() throw(CException) {
    EVTDISPAPP_ << "start"; 
}

    //!DefaultEventDispatcher deinitialization
/*!
 * All rpc adapter and command dispatcher are deinitilized. All instantiated channel are disposed
 */
void DefaultEventDispatcher::deinit() throw(CException) {
    EVTDISPAPP_ << "Deinit";
    if(alertEventScheduler) {
        alertEventScheduler->deinit();
        delete(alertEventScheduler);
    }
    
    if(instrumentEventScheduler) {
        instrumentEventScheduler->deinit();
        delete(instrumentEventScheduler);
    }

}

    //! Event handler registration
/*
 Perform the registration of an handler
 */
void DefaultEventDispatcher::registerEventAction(EventAction *eventAction, EventType eventType, const char * const idntification)  throw(CException) {
    boost::shared_lock<boost::shared_mutex> lock(handlerVEctorMutext);
    
    EVTDISPAPP_ << "registerEventActionForEventType";
    if(!eventAction) throw new CException(0, "The action pointer is null", "DefaultEventDispatcher::registerEventActionForEventType");
    
    switch (eventType) {
        case event::EventTypeAlert:
            alertEventScheduler->installEventAction(eventAction);
            break;
            
        case event::EventTypeInstrument:
            instrumentEventScheduler->installEventAction(eventAction);
            break;
        default:
            break;
    }
}

    //! Event handler deregistration
/*
 Perform the deregistration of an handler
 */
void DefaultEventDispatcher::deregisterEventAction(EventAction *eventAction)  throw(CException) {
    EVTDISPAPP_ << "deregisterEventAction";
        //try to remove from all scheduler, becaus eone action can be mapped to all event type
    if(alertEventScheduler) alertEventScheduler->removeEventAction(eventAction);
    
    if(instrumentEventScheduler) instrumentEventScheduler->installEventAction(eventAction);
}

/*!
 Thsi is the methdo that is called when the specified event is recognized
 by dispatcher
 */
void DefaultEventDispatcher::executeAlertHandler(alert::AlertEventDescriptor *eventDescription)  throw(CException) {
    EVTDISPAPP_ << "Received and alert event";
    alertEventScheduler->push(eventDescription);
}

    //!Handler execution method
/*!
 Thsi is the methdo that is called when the specified event is recognized
 by dispatcher
 */
void DefaultEventDispatcher::executeInstrumentHandler(instrument::InstrumentEventDescriptor *eventDescription)  throw(CException) {
     EVTDISPAPP_ << "Received an instrument event";
    alertEventScheduler->push(eventDescription);
}

    //!Handler execution method
/*!
 Thsi is the methdo that is called when the specified event is recognized
 by dispatcher
 */
void DefaultEventDispatcher::executeCommandHandler(command::CommandEventDescriptor *eventDescription)  throw(CException) {
     EVTDISPAPP_ << "executeCommandHandler";
}

    //!Handler execution method
/*!
 Thsi is the methdo that is called when the specified event is recognized
 by dispatcher
 */
void DefaultEventDispatcher::executeCustomHandler(custom::CustomEventDescriptor* eventDescription)  throw(CException) {
     EVTDISPAPP_ << "executeCustomHandler";
}
