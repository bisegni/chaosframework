//
//  DomainActions.h
//  ControlSystemLib
//
//  Created by bisegni on 07/07/11.
//  Copyright 2011 INFN. All rights reserved.
//

#ifndef ChaosLib_DomainActions_h
#define ChaosLib_DomainActions_h

#include <map>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include "ActionDescriptor.h"
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/exception/CException.h>

namespace chaos {
    using namespace std;
    using namespace boost;
    
    /*
     This class contains all action description for a domain,
     al action to be executed is send to a queued buffer to be executed in fifo login 
     into a separated thread
     */
    class DomainActions {
        string domainName;
        //mutex actionAccessMutext;
        map<string, AbstActionDescShrPtr> actionDescriptionsMap;
    public:
        DomainActions(const char*const);
        DomainActions(string&);
        ~DomainActions();
        
        /*
         Add an action descriptor to this domain
         */
        bool addActionDescriptor(AbstActionDescShrPtr) throw(CException);
        
        /*
         remove an action identified by it's descriptor
         */
        void removeActionDescriptor(AbstActionDescShrPtr) throw(CException);

        /*
         return the domain name
         */
        string& getDomainName();
        
        /*
         check for action name presence
         */
        bool hasActionName(string&);
        
        /*
         return the reference to action desciptor ssociated with action name
         for managed domain
         */
        AbstActionDescShrPtr& getActionDescriptornFormActionName(string&);
    };
    
}

#endif
