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

#include <chaos/common/utility/StartableService.h>
#include <chaos/common/event/EventHandler.h>
#include <chaos/common/utility/NamedService.h>
#include <chaos/common/action/EventAction.h>
#include <chaos/common/event/evt_desc/EventDescriptor.h>

using namespace chaos::event;
using namespace chaos::utility;

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
   
    class AbstractEventDispatcher : public utility::StartableService, event::EventHandler, NamedService {
        friend class chaos::common::network::NetworkBroker;

    public:
        AbstractEventDispatcher(string alias);
        
        ~AbstractEventDispatcher();
            //! Event handler registration
        /*
            Perform the registration of an handler
         */
        virtual void registerEventAction(EventAction *eventAction, EventType eventType, const char * const identificationString = NULL)  throw(CException) = 0;
        
            //! Event handler deregistration
        /*
            Perform the deregistration of an handler
         */
        virtual void deregisterEventAction(EventAction *eventAction)  throw(CException) = 0;
    };
    
}
#endif /* defined(__CHAOSFramework__AbstractEventDispatcher__) */
