/*	
 *	DomainActionsScheduler.h
 *	!CHOAS
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
#ifndef ChaosFramework_ActionsScheduler_h
#define ChaosFramework_ActionsScheduler_h

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include <chaos/common/action/DomainActions.h>
#include "CommandDispatcher.h"
#include <chaos/common/pqueue/ChaosProcessingQueue.h>
#include <chaos/common/data/CDataWrapper.h>

namespace chaos {
    using namespace std;
    using namespace boost;
    
    /*
     This class define an environment where an aciotn can be executed
     */
    class DomainActionsScheduler: public CObjectProcessingQueue<CDataWrapper> {
        friend class CommandDispatcher;
        bool armed;
        boost::mutex actionAccessMutext;
        shared_ptr<DomainActions> domainActionsContainer;
        
            //reference to global dispatcher used
            //to resubmit sub command
        CommandDispatcher *dispatcher;
    protected:
        virtual void processBufferElement(CDataWrapper*, ElementManagingPolicy&) throw(CException);
        
    public:
        /*
         
         */
        DomainActionsScheduler(shared_ptr<DomainActions>);
        /*
         
         */
        string& getManagedDomainName();
        
        /*
         Initialization method for output buffer
         */
        virtual void init(int) throw(CException);
        
        /*
         Deinitialization method for output buffer
         */
        virtual void deinit() throw(CException);
        /*
         
         */
        bool push(CDataWrapper*) throw(CException);
        /*
         
         */
        void setDispatcher(CommandDispatcher*);
    };
}
#endif
