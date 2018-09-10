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

#ifndef __CHAOSFramework__EventActionHandler__
#define __CHAOSFramework__EventActionHandler__

#include <chaos/common/event/evt_desc/AlertEventDescriptor.h>
#include <chaos/common/event/evt_desc/InstrumentEventDescriptor.h>
#include <chaos/common/event/evt_desc/CommandEventDescriptor.h>
#include <chaos/common/event/evt_desc/CustomEventDescriptor.h>

namespace chaos {
    class AbstractEventDispatcer;
    namespace common {
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
                EventHandler(){};
                virtual ~EventHandler(){};
                //!Alert handler execution method
                /*!
                 Thsi is the methdo that is called when the specified event is recognized
                 by dispatcher
                 */
                virtual void executeAlertHandler(alert::AlertEventDescriptor *eventDescription) = 0;
                //!Instrument handler execution method
                /*!
                 Thsi is the methdo that is called when the specified event is recognized
                 by dispatcher
                 */
                virtual void executeInstrumentHandler(instrument::InstrumentEventDescriptor *eventDescription) = 0;
                //!Command handler execution method
                /*!
                 Thsi is the methdo that is called when the specified event is recognized
                 by dispatcher
                 */
                virtual void executeCommandHandler(command::CommandEventDescriptor *eventDescription) = 0;
                //!Custom handler execution method
                /*!
                 Thsi is the methdo that is called when the specified event is recognized
                 by dispatcher
                 */
                virtual void executeCustomHandler(custom::CustomEventDescriptor* eventDescription) = 0;
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__EventActionHandler__) */
