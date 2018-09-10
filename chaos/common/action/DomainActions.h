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
#ifndef ChaosFramework_DomainActions_h
#define ChaosFramework_DomainActions_h

#include <map>
#include <string>

#include <chaos/common/action/ActionDescriptor.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/exception/exception.h>

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
        DomainActions(const string & domain_name);
        ~DomainActions();
        
        /*
         Add an action descriptor to this domain
         */
        bool addActionDescriptor(AbstActionDescShrPtr);
        
        /*
         remove an action identified by it's descriptor
         */
        void removeActionDescriptor(AbstActionDescShrPtr);

        /*
         return the domain name
         */
        const string& getDomainName();
        
        /*
         check for action name presence
         */
        bool hasActionName(const string&);
        
		//! return the number of the registered action in the domain
		uint32_t registeredActions();
		
        /*
         return the reference to action desciptor ssociated with action name
         for managed domain
         */
        AbstActionDescShrPtr& getActionDescriptornFormActionName(const string&);
    };
    
}
#endif
