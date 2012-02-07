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

#include <boost/chrono.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/recursive_mutex.hpp>

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
    
    //! Sandbox for the controlunit execution
    /*! 
     This is Sandbox container where Control Unite live. It abstract all dataserver comunication to Control Unit, take care
     to proxy all metadata server <-> Control unit comunication.
     */
    class ControlUnitSandbox : public CThreadExecutionTask, public DeclareAction {
        
        friend class ControlManager;
        
        //! Delay between heartbeat event in microseconds
        boost::chrono::microseconds heartBeatDelayInMicroseconds;
        
        //! Last heartbeat time
        high_resolution_clock::time_point lastHeartBeatTime;
        
        //! Sand box generated name
        string sandboxName;
        
        //! Control Unit instance managed by this sandbox
        AbstractControlUnit *acu;
        
        //!CThread utility class instance
        CThread *csThread;
        
        //!mutex for multithreading managment of sand box
        /*!
         The muthex is needed because the call to the action can occours in different thread
         */
        recursive_mutex managing_cu_mutex;
        
        /*!
         \return the managed control unit name
         */
        const char * getCUName();
        
        /*!
         \return the managed control unit instance id
         */
        const char * getCUInstance();
        
        /*!
         Add the KeyDataStorage for managed devices 
         */
        void addKeyDataStorageToCU(CDataWrapper*);
        
        /*!
         Perform the heartbeat operation for managed Contorl Unit Instance
         */
        void performHeartbeat();
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
         Define control unit
         */
        void undefineSandboxAndControlUnit() throw (CException); 
        
        /*
         Sand box initialization, this method take care to setup the control unit
         */
        CDataWrapper* initSandbox(CDataWrapper*, bool&) throw (CException);
        /*
         Sand box Deinitialization, this method take care to deinitialize all the subtree 
         starting from CU->buffer->iodriver
         */
        CDataWrapper* deinitSandbox(CDataWrapper*, bool&) throw (CException);
        
        /*
         Start the sandbox
         */
        CDataWrapper* startSandbox(CDataWrapper*, bool&) throw (CException);
        
        /*
         Stop the sandbox
         */
        CDataWrapper* stopSandbox(CDataWrapper*, bool&) throw (CException);
        
        /*
         Configure the sandbox and all subtree of the CU
         */
        CDataWrapper* updateConfiguration(CDataWrapper*, bool&);
        
        /*
         */
        CDataWrapper *getInternalCUConfiguration(){
            CHAOS_ASSERT(acu)
            return acu->_internalSetupConfiguration.get();
        }
    };
}


#endif
