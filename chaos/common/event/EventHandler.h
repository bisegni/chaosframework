/*
 *	EventHandler.h
 *	!CHAOS
 *	Created by Claudio Bisegni on 20/08/12.
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

#ifndef __CHAOSFramework__EventActionHandler__
#define __CHAOSFramework__EventActionHandler__

#include <chaos/common/event/evt_desc/AlertEventDescriptor.h>
#include <chaos/common/event/evt_desc/InstrumentEventDescriptor.h>
#include <chaos/common/event/evt_desc/CommandEventDescriptor.h>
#include <chaos/common/event/evt_desc/CustomEventDescriptor.h>

namespace chaos {
    class AbstractEventDispatcer;
    
    namespace event {
            //!Define an action to be axecuted afetr a specific event is received
        /*!
         Thsi class represent an handler that is called when a specified event is get
         from the dispatcher.
         */
        class EventHandler {
            friend class EventServer;
            friend class AbstractEventDispatcer;
        protected:
                //!Alert handler execution method
            /*!
             Thsi is the methdo that is called when the specified event is recognized
             by dispatcher
             */
            virtual void executeAlertHandler(alert::AlertEventDescriptor *eventDescription) throw(CException) = 0;
                //!Instrument handler execution method
            /*!
             Thsi is the methdo that is called when the specified event is recognized
             by dispatcher
             */
            virtual void executeInstrumentHandler(instrument::InstrumentEventDescriptor *eventDescription) throw(CException) = 0;
                //!Command handler execution method
            /*!
             Thsi is the methdo that is called when the specified event is recognized
             by dispatcher
             */
            virtual void executeCommandHandler(command::CommandEventDescriptor *eventDescription) throw(CException) = 0;
                //!Custom handler execution method
            /*!
             Thsi is the methdo that is called when the specified event is recognized
             by dispatcher
             */
            virtual void executeCustomHandler(custom::CustomEventDescriptor* eventDescription) throw(CException) = 0;
        };
    }
}

#endif /* defined(__CHAOSFramework__EventActionHandler__) */
