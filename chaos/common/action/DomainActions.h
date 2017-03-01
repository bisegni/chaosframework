/*	
 *	DomainActions.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
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
        bool addActionDescriptor(AbstActionDescShrPtr) throw(CException);
        
        /*
         remove an action identified by it's descriptor
         */
        void removeActionDescriptor(AbstActionDescShrPtr) throw(CException);

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
