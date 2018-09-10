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


#include <chaos/common/dispatcher/DefaultEventDispatcher.h>
#include <chaos/common/dispatcher/EventTypeScheduler.h>

using namespace chaos;
using namespace chaos::common::event;

#define EVTDISPAPP_ INFO_LOG(DefaultEventDispatcher)
#define EVTDISPDBG_ DBG_LOG(DefaultEventDispatcher)
#define EVTDISPERR_ ERR_LOG(DefaultEventDispatcher)

DEFINE_CLASS_FACTORY(DefaultEventDispatcher, AbstractEventDispatcher);

    //! Basic Constructor
DefaultEventDispatcher::DefaultEventDispatcher(string alias):
AbstractEventDispatcher(alias),
eventScheduler(NULL){
    
}

    //! Basic Destructor
DefaultEventDispatcher::~DefaultEventDispatcher() {
    
}

void DefaultEventDispatcher::init(void* initData) {
    eventScheduler = new EventTypeScheduler();
    if(!eventScheduler) throw CException(0, "Error allocating Alert Event Scheduler", "DefaultEventDispatcher::init");
    eventScheduler->init();

}

void DefaultEventDispatcher::start() {
}

    //!DefaultEventDispatcher deinitialization
/*!
 * All rpc adapter and command dispatcher are deinitilized. All instantiated channel are disposed
 */
void DefaultEventDispatcher::deinit() {
    if(eventScheduler) {
        eventScheduler->deinit();
        DELETE_OBJ_POINTER(eventScheduler);
    }
}

    //! Event handler registration
/*
 Perform the registration of an handler
 */
void DefaultEventDispatcher::registerEventAction(EventAction *eventAction, EventType eventType, const char * const idntification)  {
    boost::shared_lock<boost::shared_mutex> lock(handlerVEctorMutext);
    
    EVTDISPAPP_ << "registerEventActionForEventType";
    if(!eventAction) {throw CException(0, "The action pointer is null", "DefaultEventDispatcher::registerEventActionForEventType");}
    
    eventScheduler->installEventAction(eventAction);
}

    //! Event handler deregistration
/*
 Perform the deregistration of an handler
 */
void DefaultEventDispatcher::deregisterEventAction(EventAction *eventAction)  {
    EVTDISPAPP_ << "deregisterEventAction";
        //try to remove from all scheduler, becaus eone action can be mapped to all event type
    if(eventScheduler) {eventScheduler->removeEventAction(eventAction);}
}

/*!
 Thsi is the methdo that is called when the specified event is recognized
 by dispatcher
 */
void DefaultEventDispatcher::executeAlertHandler(alert::AlertEventDescriptor *eventDescription)  {
    eventScheduler->push(eventDescription);
}

    //!Handler execution method
/*!
 Thsi is the methdo that is called when the specified event is recognized
 by dispatcher
 */
void DefaultEventDispatcher::executeInstrumentHandler(instrument::InstrumentEventDescriptor *eventDescription)  {
    eventScheduler->push(eventDescription);
}

    //!Handler execution method
/*!
 Thsi is the methdo that is called when the specified event is recognized
 by dispatcher
 */
void DefaultEventDispatcher::executeCommandHandler(command::CommandEventDescriptor *eventDescription)  {
}

    //!Handler execution method
/*!
 Thsi is the methdo that is called when the specified event is recognized
 by dispatcher
 */
void DefaultEventDispatcher::executeCustomHandler(custom::CustomEventDescriptor* eventDescription)  {

}
