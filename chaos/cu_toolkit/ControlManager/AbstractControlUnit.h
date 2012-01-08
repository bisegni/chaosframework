//
//  ControlUnit.h
//  ChaosFramework
//
//  Created by Claudio Bisegni on 09/03/11.
//  Copyright 2011 INFN. All rights reserved.
//

#ifndef ControlUnit_H
#define ControlUnit_H

#include <map>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include <chaos/common/data/CUSchemaDB.h>
#include <chaos/common/general/Configurable.h>
#include <chaos/common/action/ActionDescriptor.h>
#include <chaos/common/exception/CException.h>
#include <chaos/common/action/DeclareAction.h>
#include <chaos/common/utility/ArrayPointer.h>
#include <chaos/cu_toolkit/DataManager/KeyDataStorage.h>

namespace chaos{
    
    #define CU_IDENTIFIER_C_STREAM getCUName() << "_" << getCUInstance() 
    
    class ControlUnitThread;
        //class ControlUnitInstantiator;
    using namespace std;

/*
 Base class for control unit execution task
 */
    class AbstractControlUnit:public Configurable, public DeclareAction, public CUSchemaDB {
        friend class ControlUnitSandbox;
        int32_t scheduleDelay;
        string jsonSetupFilePath;
            //vector<string> managedDeviceID;

            //
        map<string, KeyDataStorage*>  keyDataStorageMap;
        
        /*
         Add a new KeyDataStorage for a specific key
         */
        void addKeyDataStorage(const char *, KeyDataStorage*);
        
        void _sharedInit();
    public:
        /*
         Construct a new CU with an identifier
         */
        AbstractControlUnit(const char *);
        /*
         Construct a new CU with an identifier
         */
        AbstractControlUnit();
        /*
         Destructor a new CU with an identifier
         */
        virtual ~AbstractControlUnit();
        
        /*
         return the CU name
         */
        const char * getCUName();
        /*
         return the CU instance
         */
        const char * getCUInstance();      
        /*
         Define the control unit DataSet and Action into
         a CDataWrapper
         */
        void _defineActionAndDataset(CDataWrapper&) throw(CException);
        
        /*
         Initialize the Custom Contro Unit and return the configuration
         */
        void _init(CDataWrapper*) throw(CException);        
        
        /*
         Deinit the Control Unit
         */
        void _deinit() throw(CException);

protected:
            //CU Identifier
        string cuName;
            //CU instance, this te fine the current isntance code
            //it's dynamically assigned
        string cuInstance;
        
        shared_ptr<CDataWrapper> _internalSetupConfiguration;

        /*
         Return the tart configuration for the Control Unit instance
         */
        virtual void defineActionAndDataset(CDataWrapper&) throw(CException)   = 0;
      
        /*
         Initialize the Custom Contro Unit and return the configuration
         */
        virtual void init(CDataWrapper*) throw(CException) = 0;
        
        /*
         Execute the Control Unit work
         */
        virtual void run() throw(CException) = 0;
        
        /*
         Execute the Control Unit work
         */
        virtual void stop() throw(CException) = 0;
        
        /*
         Deinit the Control Unit
         */
        virtual void deinit() throw(CException) = 0;
        /*
         Receive the evento for set the dataset input element
         */
        virtual CDataWrapper* setDatasetAttribute(CDataWrapper*) throw (CException) = 0;
        
        /*
         Event for update some CU configuration
         */
        virtual CDataWrapper* updateConfiguration(CDataWrapper*) throw (CException);
        
        /*
         Receive the evento for set the dataset input element
         */
        virtual CDataWrapper* _setDatasetAttribute(CDataWrapper*) throw (CException);
        
        /*
         Create a new action description, return the description for let the user to add parameter
         */
        template<typename T>
        AbstActionDescShrPtr addActionDescritionInstance(T* actonObjectPointer, typename ActionDescriptor<T>::ActionPointerDef actionHandler, const char*const actionAliasName, const char*const actionDescription) {
            //call the DeclareAction action register method, the domain will be associated to the control unit isntance
            return DeclareAction::addActionDescritionInstance(actonObjectPointer, actionHandler, cuInstance.c_str(), actionAliasName, actionDescription);
        }
        
        //--------------Contro Unit Service Method----------------
        /*
         Init the dataset ad other values by a json file
         */
        void initWithJsonFilePath(const char*const _jsonSetupFilePath) {
            jsonSetupFilePath.assign(_jsonSetupFilePath);
        }
        
        /*
         Set the default schedule delay for the sandbox
         */
        void setDefaultScheduleDelay(int32_t _sDelay){scheduleDelay = _sDelay;};
        
        /*
         load the json file setupped into jsonSetupFilePath class attributed
         */
        void loadCDataWrapperForJsonFile(CDataWrapper&)  throw (CException);
        
        /*
         Send device data to output buffer
         */
        void pushDataSetForKey(const char *key, CDataWrapper*);
  
        /*
         get latest device data 
         */
        ArrayPointer<CDataWrapper> *getLastDataSetForKey(const char *key);

        /*
         return a new instance of CDataWrapper filled with a mandatory data
         according to key
         */
        CDataWrapper *getNewDataWrapperForKey(const char*);        
};
}
#endif
