    //
    //  ControlUnitSandbox.h
    //  ChaosFramework
    //
    //  Created by Claudio Bisegni on 21/06/11.
    //  Copyright 2011 INFN. All rights reserved.
    //

#ifndef ChaosFramework_ControlUnitSandbox_h
#define ChaosFramework_ControlUnitSandbox_h

#include <vector>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/shared_ptr.hpp>

#include <chaos/cu_toolkit/Controlmanager/AbstractControlUnit.h>
#include <chaos/common/general/Configurable.h>
#include <chaos/common/thread/ChaosThread.h>
#include <chaos/common/exception/CException.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/action/ActionDescriptor.h>
#include <chaos/common/action/DeclareAction.h>

namespace chaos {
    using namespace std;
    using namespace boost;
    
    /*
     Sandbox for the controlunit execution
     */
    class ControlUnitSandbox : public CThreadExecutionTask, public Configurable, public DeclareAction {
        
        friend class ControlManager;

        string sandboxName;
        recursive_mutex managing_cu_mutex;

        AbstractControlUnit *acu;
        CThread *csThread;
        
        
        const char * getCUName();
        const char * getCUInstance();
        void addKeyDataStorageToCU(CDataWrapper*);
    protected:
        bool started;
        bool autostart;
        bool initialized;
        void executeOnThread() throw(CException);
    public:
        /*
         Construct the CUSandBox with a Contorl Unit pointer
         */
        ControlUnitSandbox(AbstractControlUnit*);
        
        /*
         Contorl Unit Desctruction
         */
        ~ControlUnitSandbox();
        
        /*
         Return the sandbox name
         */
        string& getSandboxName();
        
        /*
         Define control unit
         */
        void defineSandboxAndControlUnit(CDataWrapper&) throw (CException); 
        
        /*
         Sand box initialization, this method take care to setup the control unit
         */
        void initSandbox(CDataWrapper*) throw (CException);
        /*
         Sand box Deinitialization, this method take care to deinitialize all the subtree 
         starting from CU->buffer->iodriver
         */
        void deinitSandbox() throw (CException);
        
        /*
         Start the sandbox
         */
        CDataWrapper* startSandbox(CDataWrapper*) throw (CException);
        
        /*
         Stop the sandbox
         */
        CDataWrapper* stopSandbox(CDataWrapper*) throw (CException);
        
        /*
         Configure the sandbox and all subtree of the CU
         */
        CDataWrapper* updateConfiguration(CDataWrapper*);
        
        /*
         */
        CDataWrapper *getInternalCUConfiguration(){
            CHAOS_ASSERT(acu)
            return acu->_internalSetupConfiguration.get();
        }
    };
}


#endif
