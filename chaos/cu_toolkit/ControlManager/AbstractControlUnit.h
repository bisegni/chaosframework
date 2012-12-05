/*	
 *	ControlUnit.h
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
#ifndef ControlUnit_H
#define ControlUnit_H
#pragma GCC diagnostic ignored "-Woverloaded-virtual"

#include <map>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/thread/recursive_mutex.hpp>

#include <chaos/common/data/CUSchemaDB.h>
#include <chaos/common/general/Configurable.h>
#include <chaos/common/action/ActionDescriptor.h>
#include <chaos/common/exception/CException.h>
#include <chaos/common/action/DeclareAction.h>
#include <chaos/common/utility/ArrayPointer.h>
#include <chaos/common/thread/CThread.h>
#include <chaos/cu_toolkit/DataManager/KeyDataStorage.h>
#include <chaos/common/pqueue/CObjectHandlerProcessingQueue.h>
#include <chaos/common/thread/CThreadExecutionTask.h>
#include <chaos/cu_toolkit/ControlManager/handler/DSAttributeHandler.h>
#include <chaos/cu_toolkit/ControlManager/DSAttributeHandlerExecutionEngine.h>
#include <boost/chrono.hpp>

namespace chaos{
    
#define CU_IDENTIFIER_C_STREAM getCUName() << "_" << getCUInstance() 
#define INIT_STATE      0
#define START_STATE     1
    
        //forward evenc channel declaration
    namespace event{
        namespace channel {
            class InstrumentEventChannel;
        }
    }
    
    class ControManager;
    class ControlUnitThread;
    
    using namespace std;
    
    class ActionData {
        
    public:
        void purge(){
            if(actionData) delete(actionData);
        }
        CDataWrapper *actionData;
        boost::function<CDataWrapper*(CDataWrapper*, bool)> actionDef;
    };
    
    /*
     Base class for control unit execution task
     */
    class AbstractControlUnit:public DeclareAction, public CUSchemaDB, public CThreadExecutionTask {
        friend class ControlManager;
        
        int32_t scheduleDelay;
        string jsonSetupFilePath;
        boost::chrono::seconds  lastAcquiredTime;
        
        std::map<std::string, cu::DSAttributeHandlerExecutionEngine*> attributeHandlerEngineForDeviceIDMap;
        
            //!mutex for multithreading managment of sand box
        /*!
         The muthex is needed because the call to the action can occours in different thread
         */
        boost::recursive_mutex managing_cu_mutex;
        
            //
        map<string, KeyDataStorage*>  keyDataStorageMap;
        
        map<string, CObjectHandlerProcessingQueue<ActionData*>* >  actionParamForDeviceMap;
        
        map<string, CThread* >  schedulerDeviceMap;
        
        map<string, boost::chrono::seconds >  heartBeatDeviceMap;
        
        map<string, int >  deviceStateMap;
        
        map<string, CUStateKey::ControlUnitState > deviceExplicitStateMap;
        
        event::channel::InstrumentEventChannel *deviceEventChannel;
        
        /*!
         Add a new KeyDataStorage for a specific key
         */
        void addKeyDataStorage(const char *, KeyDataStorage*);
        
        void _sharedInit();
    public:
        /*!
         Construct a new CU with an identifier
         */
        AbstractControlUnit(const char *);
        /*!
         Construct a new CU with an identifier
         */
        AbstractControlUnit();
        /*!
         Destructor a new CU with an identifier
         */
        virtual ~AbstractControlUnit();
        
        /*!
         return the CU name
         */
        const char * getCUName();
        /*!
         return the CU instance
         */
        const char * getCUInstance();
        /*!
         Define the control unit DataSet and Action into
         a CDataWrapper
         */
        void _defineActionAndDataset(CDataWrapper&) throw(CException);
        
        /*!
         Define the control unit DataSet and Action into
         a CDataWrapper
         */
        void _undefineActionAndDataset() throw(CException);
        
        /*!
         Initialize the Custom Contro Unit and return the configuration
         */
        CDataWrapper* _init(CDataWrapper*, bool& detachParam) throw(CException);
        
        /*!
         Deinit the Control Unit
         */
        CDataWrapper* _deinit(CDataWrapper*, bool& detachParam) throw(CException);
        
        /*!
         Starto the  Control Unit scheduling for device
         */
        CDataWrapper* _start(CDataWrapper*, bool& detachParam) throw(CException);
        
        /*!
         Stop the Custom Control Unit scheduling for device
         */
        CDataWrapper* _stop(CDataWrapper*, bool& detachParam) throw(CException);
        
        /*!
         Receive the evento for set the dataset input element
         */
        virtual CDataWrapper* _setDatasetAttribute(CDataWrapper*, bool&) throw (CException);
        
        /*!
         Get the current control unit state
         */
        CDataWrapper* _getState(CDataWrapper*, bool& detachParam) throw(CException);

    protected:
            //CU Identifier
        string cuName;
            //CU instance, this te fine the current isntance code
            //it's dynamically assigned
        string cuInstance;
        
        boost::shared_ptr<CDataWrapper> _internalSetupConfiguration;
        
        /*!
         Return the tart configuration for the Control Unit instance
         */
        virtual void defineActionAndDataset(CDataWrapper&) throw(CException)   = 0;
        
        /*!
         Initialize the Custom Contro Unit and return the configuration
         */
        virtual void init(CDataWrapper*) throw(CException) = 0;
        
        /*!
         Execute the Control Unit work
         */
        virtual void run(const string&) throw(CException) = 0;
        
        /*!
         Execute the Control Unit work
         */
        virtual void stop(const string&) throw(CException) = 0;
        
        /*!
         Deinit the Control Unit
         */
        virtual void deinit(const string&) throw(CException) = 0;
        
        /*!
         Receive the event for set the dataset input element, this virtual method
         is empty because can be used by controlunit implementation
         */
        virtual CDataWrapper* setDatasetAttribute(CDataWrapper*, bool&) throw (CException){return NULL;};
        
        /*!
         Event for update some CU configuration
         */
        virtual CDataWrapper* updateConfiguration(CDataWrapper*, bool&) throw (CException);

        
        /*!
         Execute the scehduling for the device
         */
        void executeOnThread(const string&) throw(CException);
        
        /*!
         Create a new action description, return the description for let the user to add parameter
         */
        template<typename T>
        AbstActionDescShrPtr addActionDescritionInstance(T* actonObjectPointer, typename ActionDescriptor<T>::ActionPointerDef actionHandler, const char*const actionAliasName, const char*const actionDescription) {
                //call the DeclareAction action register method, the domain will be associated to the control unit isntance
            return DeclareAction::addActionDescritionInstance(actonObjectPointer, actionHandler, cuInstance.c_str(), actionAliasName, actionDescription);
        }
        
            //! add an handler for a determinate device id and attribute set
        /*!
         *  This function permit to attach an handler to an attribute name. When the control unit
         *  receive by RPC an evento to set, a determinate attribute, to an specified value, this
         *  handler will be called.
         *  \param deviceID is the identification of the device that contain the attribute
         *  \param attrName is the name of the attribute where the handler need to be attached
         *  \param classHandler is the pointer to handler that need to be attached
         *  \exception something is gone wrong
         */
        void addHandlerForDSAttribute(const char * deviceID, cu::handler::DSAttributeHandler * classHandler)  throw (CException);
        
            //--------------Contro Unit Service Method----------------
        /*!
         Init the dataset ad other values by a json file
         */
        void initWithJsonFilePath(const char*const _jsonSetupFilePath) {
            jsonSetupFilePath.assign(_jsonSetupFilePath);
        }
        
        /*!
         Set the default schedule delay for the sandbox
         */
        void setDefaultScheduleDelay(int32_t _sDelay){scheduleDelay = _sDelay;};
        
        /*!
         load the json file setupped into jsonSetupFilePath class attributed
         */
        void loadCDataWrapperForJsonFile(CDataWrapper&)  throw (CException);
        
        /*!
         Send device data to output buffer
         */
        void pushDataSetForKey(const char *key, CDataWrapper*);
        
        /*!
         get latest device data 
         */
        ArrayPointer<CDataWrapper> *getLastDataSetForKey(const char *key);
        
        /*!
         return a new instance of CDataWrapper filled with a mandatory data
         according to key
         */
        CDataWrapper *getNewDataWrapperForKey(const char*);        
    };
}
#endif
