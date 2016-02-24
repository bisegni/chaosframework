//
//  EventAction.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 29/08/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#ifndef CHAOSFramework_EventAction_h
#define CHAOSFramework_EventAction_h


#include <string>
#include <map>
#include <boost/thread/locks.hpp>
#include <chaos/common/event/evt_desc/EventDescriptor.h>
#include <chaos/common/action/ActionExecutionSafetySystem.h>

namespace chaos {
    
    using namespace std;
    class EventTypeScheduler;

    
        //! Define an action for an event
    /*!
     
     */
    class EventAction : public ActionExecutionSafetySystem {
        friend class EventTypeScheduler;
            //!unique identifier for that action instance
        string actionUUID;
            //!the domain name of this action system
        string domainName;
            //! this map an identifier name to a boolean that identify the
            /*! if this cation can be triggered is due to at the presence of the indetifier with a boolean
                if this map is empty
             */
        map<string,bool> indetifierExecutioTrigger;
        
            //! mutex for mana identifier map
        boost::shared_mutex  identifierMutext;
    protected:
        
            //--------------------------------
        virtual void handleEvent(const common::event::EventDescriptor * const event) = 0;

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
        
        /*
         */
        void registerForIdentifier(const char * newIdentifier);
        
        /*
         */
        void deregisterForIdentifier(const char * newIdentifier);
        
        /*
         */
        void clearAllRegisteredIdentifier(const char * newIdentifier);
        
        /*
         */
        bool hasIdentifier(const char * newIdentifier);
    };
}

#endif
