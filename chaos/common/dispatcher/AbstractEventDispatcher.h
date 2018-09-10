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

#ifndef __CHAOSFramework__AbstractEventDispatcher__
#define __CHAOSFramework__AbstractEventDispatcher__

#include <chaos/common/utility/StartableService.h>
#include <chaos/common/event/EventHandler.h>
#include <chaos/common/utility/NamedService.h>
#include <chaos/common/action/EventAction.h>
#include <chaos/common/event/evt_desc/EventDescriptor.h>

namespace chaos {
    namespace common {
        namespace network {
            class NetworkBroker;
        }
    }

        //! Abstract class for the event dispatcher
    /*!
     This is the base class that set the rule for create an event dispatcher. The event dipsatcher has the
     scope to forward the event received from the implemntation of the event server, to internal handler 
     that has been register for specified kind of event.
     */
   
    class AbstractEventDispatcher:
	public common::utility::StartableService,
	public common::utility::NamedService,
    public common::event::EventHandler {
        friend class chaos::common::network::NetworkBroker;

    public:
        AbstractEventDispatcher(string alias);
        
        ~AbstractEventDispatcher();
            //! Event handler registration
        /*
            Perform the registration of an handler
         */
        virtual void registerEventAction(chaos::EventAction *eventAction,
                                         chaos::common::event::EventType eventType,
										 const char * const identificationString = NULL)  = 0;
        
            //! Event handler deregistration
        /*
            Perform the deregistration of an handler
         */
        virtual void deregisterEventAction(chaos::EventAction *eventAction)  = 0;
    };
    
}
#endif /* defined(__CHAOSFramework__AbstractEventDispatcher__) */
