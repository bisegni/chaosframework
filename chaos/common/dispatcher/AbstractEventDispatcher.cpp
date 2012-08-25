/*
 *	AbstractEventDispatcher.h
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
#include "AbstractEventDispatcher.h"

using namespace chaos;
using namespace chaos::event;

    //! Basic Constructor
AbstractEventDispatcher::AbstractEventDispatcher(string *alias):NamedService(alias) {
    
}

    //! Basic Destructor
AbstractEventDispatcher::~AbstractEventDispatcher() {
    
}

void AbstractEventDispatcher::init() throw(CException) {
    
}

    //!AbstractEventDispatcher deinitialization
/*!
 * All rpc adapter and command dispatcher are deinitilized. All instantiated channel are disposed
 */
void AbstractEventDispatcher::deinit() throw(CException) {
    
}


    //! Action registration
/*
 
 */
void AbstractEventDispatcher::registerHandlerForEventFilter(EventHandler *handlerToRegister)  throw(CException) {
    
}

    //! Event registration deregistration
/*
 
 */
void AbstractEventDispatcher::deregisterHanlder(EventHandler *handlerToRemove)  throw(CException) {
    
}

/*!
 Thsi is the methdo that is called when the specified event is recognized
 by dispatcher
 */
void AbstractEventDispatcher::executeAlertHandler(alert::AlertEventDescriptor *eventDescription)  throw(CException) {
    
}

    //!Handler execution method
/*!
 Thsi is the methdo that is called when the specified event is recognized
 by dispatcher
 */
void AbstractEventDispatcher::executeInstrumentHandler(instrument::InstrumentEventDescriptor *eventDescription)  throw(CException) {
    
}

    //!Handler execution method
/*!
 Thsi is the methdo that is called when the specified event is recognized
 by dispatcher
 */
void AbstractEventDispatcher::executeCommandHandler(command::CommandEventDescriptor *eventDescription)  throw(CException) {
    
}

    //!Handler execution method
/*!
 Thsi is the methdo that is called when the specified event is recognized
 by dispatcher
 */
void AbstractEventDispatcher::executeCustomHandler(custom::CustomEventDescriptor* eventDescription)  throw(CException) {
    
}
