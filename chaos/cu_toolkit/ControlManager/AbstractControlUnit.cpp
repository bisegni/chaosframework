    //
    //  ControlUnit.cpp
    //  ChaosFramework
    //
    //  Created by Claudio Bisegni on 09/03/11.
    //  Copyright 2011 INFN. All rights reserved.
    //


#include <chaos/common/global.h>
#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/cu_toolkit/ControlManager/AbstractControlUnit.h>
#include <chaos/cu_toolkit/DataManager/DataManager.h>
#include <chaos/cu_toolkit/CommandManager/CommandManager.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <boost/lexical_cast.hpp>

using namespace chaos;
using namespace std;
using namespace boost;
using namespace boost::uuids;


#define LCU_ LAPP_ << "[Control Unit:"<<getCUInstance()<<"] - "



#pragma mark constructor

AbstractControlUnit::AbstractControlUnit(){
    _sharedInit();
}

AbstractControlUnit::AbstractControlUnit(const char *descJsonPath){
    _sharedInit();
    initWithJsonFilePath(descJsonPath);
}

AbstractControlUnit::~AbstractControlUnit() {
}

/*
 thi is the shared intiialization phase of the
 abstract control unit
 */
void AbstractControlUnit::_sharedInit() {
    cuInstance = UUIDUtil::generateUUIDLite();
}

/*
 return the CU name
 */
const char * AbstractControlUnit::getCUName(){
    return cuName.size()?cuName.c_str():"Name no yet configured";
};

/*
 return the CU name
 */
const char * AbstractControlUnit::getCUInstance(){
    return cuInstance.size()?cuInstance.c_str():"Instance no yet configured";
};

/*
 Add a new KeyDataStorage for a specific key
 */
void AbstractControlUnit::addKeyDataStorage(const char *key, KeyDataStorage* keyDatStorafePointer) {
    keyDataStorageMap.insert(make_pair(key, keyDatStorafePointer));
}

/*
 load the json file setupped into jsonSetupFilePath class attributed
 */
void AbstractControlUnit::loadCDataWrapperForJsonFile(CDataWrapper& setupConfiguration)  throw (CException)  {
    int tmpInt;
    string tmpStr;
    auto_ptr<CDataWrapper> csDWFromJson;
        //CHAOS_ASSERT(setupConfiguration)
        //check if the path is filled
    if(!jsonSetupFilePath.size()) return;
    
    FILE *cfgFile = fopen(jsonSetupFilePath.c_str(), "r");
        //inFile.open(jsonSetupFilePath.c_str(), ios::binary );
    if(cfgFile){
            //check if the shared pointer if allcoated
        fseek (cfgFile, 0, SEEK_END	 );
        long length = ftell(cfgFile);
        rewind (cfgFile);
        
            //need to make 1 byte larger for simualte c string
        char *buffer = new char[length];
        memset(buffer, 0, length);
        fread(buffer, 1, length, cfgFile);
        fclose (cfgFile);
        
        while(buffer[length-1]=='\r' || buffer[length-1]=='\n'){
            buffer[(length--)-1]=(char)0;
        }
        
            //create the csdatafrapper and append all elemento to the input setupConfiguration
        csDWFromJson.reset(new CDataWrapper(buffer, false));
            //delete the buffer
        delete[] buffer;
        
        
            //add default value from jfg file
        if(csDWFromJson->hasKey(CUDefinitionKey::CS_CM_CU_NAME)){
        	tmpStr = csDWFromJson->getStringValue(CUDefinitionKey::CS_CM_CU_NAME);
        	setupConfiguration.addStringValue(CUDefinitionKey::CS_CM_CU_NAME, tmpStr);
        }
        
        if(csDWFromJson->hasKey(CUDefinitionKey::CS_CM_CU_DESCRIPTION)){
        	tmpStr = csDWFromJson->getStringValue(CUDefinitionKey::CS_CM_CU_DESCRIPTION);
        	setupConfiguration.addStringValue(CUDefinitionKey::CS_CM_CU_DESCRIPTION, tmpStr);
        }
        if(csDWFromJson->hasKey(CUDefinitionKey::CS_CM_CU_CLASS)){
            tmpStr = csDWFromJson->getStringValue(CUDefinitionKey::CS_CM_CU_CLASS);
            setupConfiguration.addStringValue(CUDefinitionKey::CS_CM_CU_CLASS, tmpStr);
        }
        
        if(csDWFromJson->hasKey(CUDefinitionKey::CS_CM_CU_AUTOSTART)){
            tmpInt = csDWFromJson->getInt32Value(CUDefinitionKey::CS_CM_CU_AUTOSTART);
            setupConfiguration.addInt32Value(CUDefinitionKey::CS_CM_CU_AUTOSTART, tmpInt);
        }
        if(csDWFromJson->hasKey(CUDefinitionKey::CS_CM_THREAD_SCHEDULE_DELAY)){
            tmpStr = csDWFromJson->getStringValue(CUDefinitionKey::CS_CM_THREAD_SCHEDULE_DELAY);
            setupConfiguration.addStringValue(CUDefinitionKey::CS_CM_THREAD_SCHEDULE_DELAY, tmpStr);
        }
        
        
            //auto_add all possible dataset attribute from the configuration file
        if(csDWFromJson->hasKey(DatasetDefinitionkey::CS_CM_DATASET_DESCRIPTION)) {
                //in the configuration file there are some attribute for the dataset we need to register it
        	auto_ptr<CMultiTypeDataArrayWrapper> datasetAttributes (csDWFromJson->getVectorValue(DatasetDefinitionkey::CS_CM_DATASET_DESCRIPTION));
        	for (unsigned int i = 0;  i < datasetAttributes->size() ; i++) {
        		addAttributeToDataSetFromDataWrapper(*datasetAttributes->getCDataWrapperElementAtIndex(i));
			}
        }
    }
}


/*
 fill the CDataWrapper with AbstractCU system configuration, this method
 is called after getStartConfiguration directly by sandbox. in this method
 are defined the action for the input element of the dataset
 */
void AbstractControlUnit::_defineActionAndDataset(CDataWrapper& setupConfiguration)  throw(CException) {
    vector<string> tempStringVector;
        //global var initialization
    scheduleDelay = 0;
    
        //add the CU isntance, this can be redefinide by user in the defineActionAndDataset method
        //for let the CU have the same instance at every run
    setupConfiguration.addStringValue(CUDefinitionKey::CS_CM_CU_INSTANCE, cuInstance);
        //check if as been setuped a file for configuration
    LCU_ << "Check if as been setup a json file path to configura CU:" << CU_IDENTIFIER_C_STREAM;
    loadCDataWrapperForJsonFile(setupConfiguration);
    
        //first call the setup abstract method used by the implementing CU to define action, dataset and other
        //usefull value
    LCU_ << "Define Actions and Dataset for:" << CU_IDENTIFIER_C_STREAM;
    defineActionAndDataset(setupConfiguration);    
    
    
        //add the scekdule dalay for the sandbox
    if(scheduleDelay){
            //in this case ovverrride the config file
    	setupConfiguration.addInt32Value(CUDefinitionKey::CS_CM_THREAD_SCHEDULE_DELAY , scheduleDelay);
    }
        //for now we need only to add custom action for expose to rpc
        //input element of the dataset
    LCU_ << "Define the base action for map the input attribute of the dataset of the CU:" << CU_IDENTIFIER_C_STREAM;
    AbstActionDescShrPtr 
    actionDescription = addActionDescritionInstance<AbstractControlUnit>(this, 
                                                                         &AbstractControlUnit::_setDatasetAttribute, 
                                                                         "setDatasetAttribute", 
                                                                         "method for set the input element for the dataset");
    
        //expose updateConfiguration Methdo to rpc
    LCU_ << "Register updateConfiguration action";
    DeclareAction::addActionDescritionInstance<AbstractControlUnit>(this, 
                                                                    &AbstractControlUnit::updateConfiguration, 
                                                                    getCUInstance(), 
                                                                    "updateConfiguration", 
                                                                    "Update Configuration");
    
    LCU_ << "Register initDevice action";
    DeclareAction::addActionDescritionInstance<AbstractControlUnit>(this, 
                                                                    &AbstractControlUnit::_init, 
                                                                    getCUInstance(), 
                                                                    ChaosSystemDomainAndActionLabel::ACTION_DEVICE_INIT, 
                                                                    "Perform the device initialization");
    
    LCU_ << "Register deinitDevice action";
    DeclareAction::addActionDescritionInstance<AbstractControlUnit>(this, 
                                                                    &AbstractControlUnit::_deinit, 
                                                                    getCUInstance(), 
                                                                    ChaosSystemDomainAndActionLabel::ACTION_DEVICE_DEINIT, 
                                                                    "Perform the device deinitialization");
    LCU_ << "Register startDevice action";
    DeclareAction::addActionDescritionInstance<AbstractControlUnit>(this, 
                                                                    &AbstractControlUnit::_start, 
                                                                    getCUInstance(), 
                                                                    ChaosSystemDomainAndActionLabel::ACTION_DEVICE_START, 
                                                                    "Sart the device scheduling");
    
    LCU_ << "Register stopDevice action";
    DeclareAction::addActionDescritionInstance<AbstractControlUnit>(this, 
                                                                    &AbstractControlUnit::_stop, 
                                                                    getCUInstance(), 
                                                                    ChaosSystemDomainAndActionLabel::ACTION_DEVICE_STOP, 
                                                                    "Stop the device scheduling");
    
    LCU_ << "Get Description for Control Unit:" << CU_IDENTIFIER_C_STREAM;
        //grab dataset description
    CUSchemaDB::fillDataWrpperWithDataSetDescirption(setupConfiguration);
    
    
        //grab action description
    LCU_ << "Get Action Description for Control Unit:" << CU_IDENTIFIER_C_STREAM;
    DeclareAction::getActionDescrionsInDataWrapper(setupConfiguration);
    
        //register command manager action
    CommandManager::getInstance()->registerAction(this);
    
    auto_ptr<SerializationBuffer> ser(setupConfiguration.getBSONData());
        //copy configuration for internal use
    _internalSetupConfiguration.reset(new CDataWrapper(ser->getBufferPtr()));
}

/*
 Define the control unit DataSet and Action into
 a CDataWrapper
 */
void AbstractControlUnit::_undefineActionAndDataset() throw(CException) {
    LCU_ << "Remove Action Description for Control Unit:" << CU_IDENTIFIER_C_STREAM;
        //register command manager action
    CommandManager::getInstance()->deregisterAction(this);
    
}

#pragma mark protected initi/deinit method
/*
 Initialize the Custom Contro Unit and return the configuration
 */
CDataWrapper* AbstractControlUnit::_init(CDataWrapper *initConfiguration, bool& detachParam) throw(CException) {
    recursive_mutex::scoped_lock  lock(managing_cu_mutex);
    KeyDataStorage *tmpKDS = 0L;
    auto_ptr<CDataWrapper> updateResult;
    if(!initConfiguration || !initConfiguration->hasKey(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID) || !initConfiguration->hasKey(DatasetDefinitionkey::CS_CM_DATASET_DESCRIPTION)) {
        throw CException(-1, "Node Device Init information in param", "AbstractControlUnit::_init");
    }
    
    string deviceID = initConfiguration->getStringValue(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID);
    LCU_ << "Initializating Phase for device:" << deviceID;
    
    if(!CUSchemaDB::deviceIsPresent(deviceID)) {
        LCU_ << "device:" << deviceID << "not known by this ContorlUnit";
        throw CException(-2, "Device not known by this control unit", "AbstractControlUnit::_init");
    }
    
        //check to see if the device can ben initialized
    if(deviceStateMap[deviceID] != INIT_STATE) {
        LCU_ << "device:" << deviceID << " already initialized";
        throw CException(-3, "Device Already Initialized", "AbstractControlUnit::_init");
    }
    
    LCU_ << "Create schedule thread for device:" << deviceID;
        //initialize device scheduler
    schedulerDeviceMap.insert(make_pair(deviceID, new CThread(this)));
    schedulerDeviceMap[deviceID]->setThreadIdentification(deviceID);
        //set the defautl scehduling rate to 1 sec
    schedulerDeviceMap[deviceID]->setDelayBeetwenTask(1000000);
        //initialize key data storage for device id
    LCU_ << "Create KeyDataStorage device:" << deviceID;
    tmpKDS = DataManager::getInstance()->getKeyDataStorageNewInstanceForKey(deviceID);
    tmpKDS->init(initConfiguration);
    
    keyDataStorageMap.insert(make_pair(deviceID, tmpKDS));
    
    LCU_ << "Start custom inititialization" << deviceID;
        //initializing the device in control unit
    init(initConfiguration);
    
        //advance status
    deviceStateMap[deviceID]++;
    
    
        //call update param function
    updateConfiguration(initConfiguration, detachParam);
    return NULL;
}

/*
 deinit all datastorage
 */
CDataWrapper* AbstractControlUnit::_deinit(CDataWrapper *deinitParam, bool& detachParam) throw(CException) {
    recursive_mutex::scoped_lock  lock(managing_cu_mutex);
    LCU_ << "Deinitializating AbstractControlUnit";
    CThread *tmpThread = 0L;
    KeyDataStorage *tmpKDS = 0L;
    
    if(!deinitParam || !deinitParam->hasKey(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID)) {
        throw CException(-1, "Node Device Defined in param", "AbstractControlUnit::_deinit");
    }
    
    string deviceID = deinitParam->getStringValue(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID);
    LCU_ << "Deinitialization Phase for device:" << deviceID;
    if(!CUSchemaDB::deviceIsPresent(deviceID)) {
        LCU_ << "device:" << deviceID << "not known by this ContorlUnit";
        throw CException(-2, "Deviuce not known by this control unit", "AbstractControlUnit::_deinit");
    }
    
        //check to see if the device can ben initialized
    if(deviceStateMap[deviceID] != INIT_STATE+1) {
        LCU_ << "device:" << deviceID << " already initialized";
        throw CException(-3, "Device Not Initialized", "AbstractControlUnit::_deinit");
    }
    
        //deinit the control unit 
    LCU_ << "Start custom deinitialization for device:" << deviceID;
    deinit(deviceID);
    
        //remove scheduler
    tmpThread = schedulerDeviceMap[deviceID];
    delete(tmpThread);
    tmpThread = 0L;
    schedulerDeviceMap.erase(deviceID);
    
        //remove key data storage
    tmpKDS = keyDataStorageMap[deviceID];
    tmpKDS->deinit();
    delete(tmpKDS);
    tmpKDS = 0L;
    keyDataStorageMap.erase(deviceID);
    
    deviceStateMap[deviceID]--;
    return NULL;
}

/*
 Starto the  Control Unit scheduling for device
 */
CDataWrapper* AbstractControlUnit::_start(CDataWrapper *startParam, bool& detachParam) throw(CException) {
    recursive_mutex::scoped_lock  lock(managing_cu_mutex);
    
    if(!startParam || !startParam->hasKey(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID)) {
        throw CException(-1, "Node Device Defined in param", "AbstractControlUnit::_start");
    }
    
    string deviceID = startParam->getStringValue(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID);
    if(!CUSchemaDB::deviceIsPresent(deviceID)) {
        LCU_ << "device:" << deviceID << "not known by this ContorlUnit";
        throw CException(-2, "Deviuce not known by this control unit", "AbstractControlUnit::_start");
    }
    
        //check to see if the device can ben initialized
    if(deviceStateMap[deviceID] != START_STATE) {
        LCU_ << "device:" << deviceID << " already strted";
        throw CException(-3, "Device already started", "AbstractControlUnit::_start");
    }            
    
    
    CThread *csThread = schedulerDeviceMap[deviceID];
    
    if(!csThread) {
        LCU_ << "No thread defined for device "<< deviceID;
        throw CException(-4, "No thread defined for device", "AbstractControlUnit::_start");
    }
    
    if(!csThread->isStopped()){
        LCU_ << "thread for "<< deviceID << "already running";
        throw CException(-5, "Thread for device already running", "AbstractControlUnit::_start");
    }
    
    
    LCU_ << "Start Thread for device id:" << deviceID;
    csThread->start();
    csThread->setThreadPriorityLevel(sched_get_priority_max(SCHED_RR), SCHED_RR);
    
    deviceStateMap[deviceID]++;
    return NULL;
}

/*
 Stop the Custom Control Unit scheduling for device
 */
CDataWrapper* AbstractControlUnit::_stop(CDataWrapper *stopParam, bool& detachParam) throw(CException) {
    recursive_mutex::scoped_lock  lock(managing_cu_mutex);
    if(!stopParam || !stopParam->hasKey(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID)) {
        throw CException(-1, "Node Device Defined in param", "AbstractControlUnit::_stop");
    }
    string deviceID = stopParam->getStringValue(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID);
    if(!CUSchemaDB::deviceIsPresent(deviceID)) {
        LCU_ << "device:" << deviceID << "not known by this ContorlUnit";
        throw CException(-2, "Device not known by this control unit", "AbstractControlUnit::_stop");
    }
    
        //check to see if the device can ben initialized
    if(deviceStateMap[deviceID] != START_STATE+1) {
        LCU_ << "device:" << deviceID << " not started";
        throw CException(-3, "Device not startded", "AbstractControlUnit::_stop");
    }            
    
    stop(deviceID);
    
    CThread *csThread = schedulerDeviceMap[deviceID];
    
    if(!csThread) {
        LCU_ << "No thread defined for device "<< deviceID;
        throw CException(-4, "No thread defined for device", "AbstractControlUnit::_stop");
    }
    
    if(csThread->isStopped()){
        LCU_ << "thread for "<< deviceID << "already runnign";
        throw CException(-5, "Thread for device already running", "AbstractControlUnit::_stop");
    }
    LCU_ << "Stopping Device ID:" << deviceID;
    csThread->stop();
    LCU_ << "Stopped Thread for Device ID:" << deviceID;
    
    deviceStateMap[deviceID]--;
    return NULL;
}

/*
 Receive the evento for set the dataset input element
 */
CDataWrapper* AbstractControlUnit::_setDatasetAttribute(CDataWrapper *datasetAttributeValues,  bool& detachParam) throw (CException) {
    CDataWrapper *executionResult = NULL;
    executionResult = setDatasetAttribute(datasetAttributeValues, detachParam);
    return executionResult;
}

/*
 Update the configuration for all descendand tree in the Control Uniti class struccture
 */
CDataWrapper*  AbstractControlUnit::updateConfiguration(CDataWrapper* updatePack, bool& detachParam) throw (CException) {
    recursive_mutex::scoped_lock  lock(managing_cu_mutex);
        //load all keyDataStorageMap for the registered devices
    if(!updatePack || !updatePack->hasKey(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID)) {
        throw CException(-1, "Update pack without DeviceID", "AbstractControlUnit::updateConfiguration");
    }
    
    string deviceID = updatePack->getStringValue(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID);
    
    if(!CUSchemaDB::deviceIsPresent(deviceID)) {
        LCU_ << "device:" << deviceID << "not known by this ContorlUnit";
        throw CException(-2, "Device not known by this control unit", "AbstractControlUnit::_stop");
    }
    
        //check to see if the device can ben initialized
    if(keyDataStorageMap.count(deviceID)!=0) {
        keyDataStorageMap[deviceID]->updateConfiguration(updatePack);
    }
    
    if(updatePack->hasKey(CUDefinitionKey::CS_CM_THREAD_SCHEDULE_DELAY)){
            //we need to configure the delay  from a run() call and the next
        int32_t uSecdelay = updatePack->getInt32Value(CUDefinitionKey::CS_CM_THREAD_SCHEDULE_DELAY);
        schedulerDeviceMap[deviceID]->setDelayBeetwenTask(uSecdelay);
    }
    
    
    return NULL;
}


/*
 Execute the scehduling for the device
 */
void AbstractControlUnit::executeOnThread( const string& deviceIDToSchedule) throw(CException) {
    run(deviceIDToSchedule);
}

#pragma mark protected API


/*
 Send device data to output buffer
 */
void AbstractControlUnit::pushDataSetForKey(const char *key, CDataWrapper *acquiredData) {
        //send data to related buffer
    keyDataStorageMap[key]->pushDataSet(acquiredData);
}

/*
 get last dataset for a specified key
 */
ArrayPointer<CDataWrapper> *AbstractControlUnit::getLastDataSetForKey(const char *key) {
        //use keydatastorage from map
    return keyDataStorageMap[key]->getLastDataSet();
}

/*
 Return a new instance of CDataWrapper filled with a mandatory data
 according to key
 */
CDataWrapper *AbstractControlUnit::getNewDataWrapperForKey(const char *key) {
    return keyDataStorageMap[key]->getNewDataWrapper();
}
