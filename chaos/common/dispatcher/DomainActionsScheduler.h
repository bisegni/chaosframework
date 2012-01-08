    //
    //  DomainActionsScheduler.h
    //  ChaosFramework
    //
    //  Created by bisegni on 08/07/11.
    //  Copyright 2011 INFN. All rights reserved.
    //

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
        virtual void processBufferElement(CDataWrapper*) throw(CException);
        
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
