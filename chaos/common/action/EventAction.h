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
    
    class EventTypeScheduler;
    
        //! Define an action for an event
    /*!
     
     */
    class EventAction {
        friend class EventTypeScheduler;
        string actionUUID;
        string domainName;
    protected:
        virtual void handleEvent(const event::EventDescriptor * const event) = 0;
    public:
       
        EventAction();
        virtual ~EventAction();
        
        /*!
         Set the name for the domain wich this action belong
         */
        void setDomainName(string& newDomainName);
        
        /*
         Return c string of the domain name
         */
        const char * const getDomainName();
        
        /*
         Return c string of the domain name
         */
        const char * const getUUID();
    };
}

#endif
