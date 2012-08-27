/*
 *	AbstractEventDispatcher.h
 *	CHAOSFramework
 *	Created by Claudio Bisegni on 26/08/12.
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

#ifndef __CHAOSFramework__AbstractEventDispatcher__
#define __CHAOSFramework__AbstractEventDispatcher__

#include <chaos/common/utility/ISDInterface.h>
#include <chaos/common/event/EventHandler.h>
#include <chaos/common/utility/NamedService.h>

namespace chaos {
    using namespace event;
    using namespace utility;

        //! Abstract class for the event dispatcher
    /*!
     This is the base class that fix the rule for create an event dispatcher. The event dipsatcher has the
     scope to forward the event received from the implemntation of the event server, to internal handler 
     that has been register for specified kind of event.
     */
   
    class AbstractEventDispatcher : public utility::ISDInterface, event::EventHandler, NamedService {
    protected:
      /*
            //-----------------------
        void init(CDataWrapper *initData) throw(CException) = 0;
        
            //-----------------------
        void start() throw(CException) = 0;
        
            //-----------------------
        void deinit() throw(CException) = 0;
        
            //-----------------------
        virtual void executeAlertHandler(alert::AlertEventDescriptor *eventDescription)  throw(CException) = 0;
        
            //-----------------------
        virtual void executeInstrumentHandler(instrument::InstrumentEventDescriptor *eventDescription)  throw(CException) = 0;
        
            //-----------------------
        virtual void executeCommandHandler(command::CommandEventDescriptor *eventDescription)  throw(CException) = 0;
        
            //-----------------------
        virtual void executeCustomHandler(custom::CustomEventDescriptor* eventDescription)  throw(CException) = 0;*/
    public:
        AbstractEventDispatcher(string *alias);
        
            //! Event handler registration
        /*
            Perform the registration of an handler
         */
        virtual void registerHandlerForEventFilter(event::EventHandler *handlerToRegister)  throw(CException) = 0;
        
            //! Event handler deregistration
        /*
            Perform the deregistration of an handler
         */
        virtual void deregisterHanlder(event::EventHandler *handlerToRemove)  throw(CException) = 0;
    };
    
}
#endif /* defined(__CHAOSFramework__AbstractEventDispatcher__) */
