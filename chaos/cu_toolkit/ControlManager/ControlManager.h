    //
    //  ControlManager.h
    //  ChaosFramework
    //
    //  Created by Claudio Bisegni on 14/06/11.
    //  Copyright 2011 INFN. All rights reserved.
    //

#ifndef ChaosFramework_ControlManager_h
#define ChaosFramework_ControlManager_h

#include <queue>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>

#include <chaos/common/general/Manager.h>
#include <chaos/cu_toolkit/Controlmanager/AbstractControlUnit.h>
#include <chaos/common/utility/Singleton.h>
#include <chaos/common/thread/ChaosThread.h>
#include <chaos/common/action/DeclareAction.h>
#include <chaos/common/exception/CException.h>
#include <chaos/common/General/Configurable.h>
#include <chaos/common/message/MDSMessageChannel.h>
namespace chaos {
    using namespace std;
    using namespace boost;
    
    /*
     Manager for the Control Unit execution
     */
    class ControlManager : public CThreadExecutionTask, public DeclareAction,  public Manager, public Singleton<ControlManager> {
        friend class Singleton<ControlManager>;
        mutable boost::mutex qMutex;
        condition_variable lockCondition;
        CThread *selfThreadPtr;
        queue< AbstractControlUnit* > submittedCUQueue;
        map<string, shared_ptr<AbstractControlUnit> > controlUnitInstanceMap;
        MDSMessageChannel *mdsChannel;
        
        void sendConfPackToMDS(CDataWrapper&);
        
        /*
         Thread method that work on buffer item
         */
        void executeOnThread(const string&) throw(CException);
        
        /*
         get the last insert data
         */
        AbstractControlUnit* waitAndPop();
        
        /*
         check for empty buffer
         */
        bool isEmpty() const;
        
    public:
        
        /*
         Constructor
         */
        ControlManager();
        
        /*
         Desctructor
         */
        ~ControlManager();
        
        /*
         Initialize the Control Manager
         */
        void init() throw(CException);
        
        /*
         Start the Control Manager
         */
        void start() throw(CException);
        
        /*
         Deinitialize the Control Manager
         */
        void deinit() throw(CException);
        
        /*
         Submit a new Control unit for operation
         */
        void submitControlUnit(AbstractControlUnit*) throw(CException); 
        
        /*
         Init the sandbox
         */
        CDataWrapper* loadControlUnit(CDataWrapper*, bool&) throw (CException);
        
        /*
         Deinit the sandbox
         */
        CDataWrapper* unloadControlUnit(CDataWrapper*, bool&) throw (CException);
        
        /*
         Configure the sandbox and all subtree of the CU
         */
        CDataWrapper* updateConfiguration(CDataWrapper*, bool&);
    };
}
#endif
