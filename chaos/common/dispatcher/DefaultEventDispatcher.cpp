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
using namespace chaos;
using namespace chaos::event;

    //! Basic Constructor
DefaultEventDispatcher::DefaultEventDispatcher(string *alias):AbstractEventDispatcher(alias) {
    
}

    //! Basic Destructor
DefaultEventDispatcher::~DefaultEventDispatcher() {
    
}

void DefaultEventDispatcher::init(CDataWrapper* initData) throw(CException) {
    
}

void DefaultEventDispatcher::start() throw(CException) {
    
}

    //!DefaultEventDispatcher deinitialization
/*!
 * All rpc adapter and command dispatcher are deinitilized. All instantiated channel are disposed
 */
void DefaultEventDispatcher::deinit() throw(CException) {
    
}


    //! Action registration
/*
 
 */
void DefaultEventDispatcher::registerHandlerForEventFilter(event::EventHandler *handlerToRegister)  throw(CException) {
    
}

    //! Event registration deregistration
/*
 
 */
void DefaultEventDispatcher::deregisterHanlder(event::EventHandler *handlerToRemove)  throw(CException) {
    
}

/*!
 Thsi is the methdo that is called when the specified event is recognized
 by dispatcher
 */
void DefaultEventDispatcher::executeAlertHandler(alert::AlertEventDescriptor *eventDescription)  throw(CException) {
    
}

    //!Handler execution method
/*!
 Thsi is the methdo that is called when the specified event is recognized
 by dispatcher
 */
void DefaultEventDispatcher::executeInstrumentHandler(instrument::InstrumentEventDescriptor *eventDescription)  throw(CException) {
    
}

    //!Handler execution method
/*!
 Thsi is the methdo that is called when the specified event is recognized
 by dispatcher
 */
void DefaultEventDispatcher::executeCommandHandler(command::CommandEventDescriptor *eventDescription)  throw(CException) {
    
}

    //!Handler execution method
/*!
 Thsi is the methdo that is called when the specified event is recognized
 by dispatcher
 */
void DefaultEventDispatcher::executeCustomHandler(custom::CustomEventDescriptor* eventDescription)  throw(CException) {
    
}
