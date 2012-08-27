//
//  EventAction.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 29/08/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#ifndef CHAOSFramework_EventAction_h
#define CHAOSFramework_EventAction_h

#include <chaos/common/event/evt_desc/EventDescriptor.h>
#include <string>

namespace chaos {
    
    using namespace std;
    
    class AbstractEventDispatcer;
    
        //! Define an action for an event
    /*!
     
     */
    class EventAction {
        friend class AbstractEventDispatcher;
        string actionUUID;
    protected:
        
        virtual void handleEvent(event::EventDescriptor *event) = 0;
    public:
        
        EventAction();
        virtual ~EventAction();
    };
}

#endif
