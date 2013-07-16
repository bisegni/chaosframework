/*
 *	ControlUnit.cpp
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

#include <chaos/common/global.h>
#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/event/channel/InstrumentEventChannel.h>
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
using namespace chaos::cu;
using namespace chaos::cu::control_manager::slow_command;
using namespace std;
using namespace boost;
using namespace boost::uuids;

#define LCU_ LAPP_ << "[Control Unit:"<<getCUInstance()<<"] - "


AbstractControlUnit::AbstractControlUnit():DeviceSchemaDB(false), cuInstance(UUIDUtil::generateUUIDLite()) {
    deviceState = 0;
    schedulerThread = NULL;
    attributeHandlerEngine = new DSAttributeHandlerExecutionEngine(this);
    slowCommandExecutor = new cu::control_manager::slow_command::SlowCommandExecutor(cuInstance, this);
}

/*!
 Destructor a new CU with an identifier
 */
AbstractControlUnit::~AbstractControlUnit() {
    if(slowCommandExecutor) {
        delete(slowCommandExecutor);
    }
    
    if(attributeHandlerEngine) {
        delete attributeHandlerEngine;
    }
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
void AbstractControlUnit::setKeyDataStorage(KeyDataStorage* keyDatStorafePointer) {
    keyDataStorage = keyDatStorafePointer;
}

/*!
 Add the new attribute in the dataset for at the CU dataset
 */
void AbstractControlUnit::addAttributeToDataSet(const char*const deviceID,
                                                const char*const attributeName,
                                                const char*const attributeDescription,
                                                DataType::DataType attributeType,
                                                DataType::DataSetAttributeIOAttribute attributeDirection,
                                                uint32_t maxDimension) {
    //add the attribute
    DeviceSchemaDB::addAttributeToDataSet(attributeName, attributeDescription, attributeType, attributeDirection, maxDimension);
}

/*
 Add a new handler
 */
void AbstractControlUnit::addHandlerForDSAttribute(const char * deviceID, cu::handler::DSAttributeHandler * classHandler)  throw (CException) {
    if(!classHandler) return;
    //add the handler
    attributeHandlerEngine->addHandlerForDSAttribute(classHandler);
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
    //LCU_ << "Check if as been setup a json file path to configura CU:" << CU_IDENTIFIER_C_STREAM;
    //loadCDataWrapperForJsonFile(setupConfiguration);
    
    //first call the setup abstract method used by the implementing CU to define action, dataset and other
    //usefull value
    LCU_ << "Define Actions and Dataset for:" << CU_IDENTIFIER_C_STREAM;
    defineActionAndDataset();
    
    
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
    LCU_ << "Register getState action";
    DeclareAction::addActionDescritionInstance<AbstractControlUnit>(this,
                                                                    &AbstractControlUnit::_getState,
                                                                    getCUInstance(),
                                                                    ChaosSystemDomainAndActionLabel::ACTION_DEVICE_GET_STATE,
                                                                    "Get the state of the device");
    
    LCU_ << "Get Description for Control Unit:" << CU_IDENTIFIER_C_STREAM;
    //grab dataset description
    DeviceSchemaDB::fillDataWrapperWithDataSetDescription(setupConfiguration);
    
#if DEBUG
    LCU_ << setupConfiguration.getJSONString();
#endif
    
    //grab action description
    LCU_ << "Get Action Description for Control Unit:" << CU_IDENTIFIER_C_STREAM;
    DeclareAction::getActionDescrionsInDataWrapper(setupConfiguration);
    
    //register command manager action
    CommandManager::getInstance()->registerAction(this);
    
    /*auto_ptr<SerializationBuffer> ser(setupConfiguration.getBSONData());
     //copy configuration for internal use
     _internalSetupConfiguration.reset(new CDataWrapper(ser->getBufferPtr()));*/
    
    //setup all state for device
    deviceExplicitState = CUStateKey::DEINIT;
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

/*!
 return the appropriate thread for the device
 */
void AbstractControlUnit::threadStartStopManagment(bool startAction) throw(CException) {
    if(startAction) {
        if(schedulerThread && !schedulerThread->isStopped()){
            LCU_ << "thread already running";
            throw CException(-5, "Thread for device already running", "AbstractControlUnit::threadStartStopManagment");
        }
        
        if(schedulerThread) {
            schedulerThread->start();
            return;
        }
        
        LCU_ << "Start Thread";
        schedulerThread = new CThread(this);
        //set the defautl scehduling rate to 1 sec
        schedulerThread->setDelayBeetwenTask(1000000);
        schedulerThread->start();
        schedulerThread->setThreadPriorityLevel(sched_get_priority_max(SCHED_RR), SCHED_RR);
    } else {
        if(schedulerThread->isStopped()){
            LCU_ << "thread already runnign";
            throw CException(-5, "Thread for device already running", "AbstractControlUnit::threadStartStopManagment");
        }
        LCU_ << "Stopping and joining scheduling thread";
        schedulerThread->stop(true);
        delete(schedulerThread);
        schedulerThread = NULL;
        LCU_ << "Thread stopped";
    }
    
}

//----------------------------------------- protected initi/deinit method ------------------------------------------------
/*
 Initialize the Custom Contro Unit and return the configuration
 */
CDataWrapper* AbstractControlUnit::_init(CDataWrapper *initConfiguration, bool& detachParam) throw(CException) {
    recursive_mutex::scoped_lock  lock(managing_cu_mutex);
    KeyDataStorage *tmpKDS = 0L;
    auto_ptr<CDataWrapper> updateResult;
    
    if(!initConfiguration ||
       !initConfiguration->hasKey(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID) ||
       !initConfiguration->hasKey(DatasetDefinitionkey::CS_CM_DATASET_DESCRIPTION)) {
        throw CException(-1, "No Device Init information in param", "AbstractControlUnit::_init");
    }
    
    string deviceID = initConfiguration->getStringValue(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID);
    LCU_ << "Initializating Phase for device:" << deviceID;
    
    if(deviceID.compare(DeviceSchemaDB::getDeviceID())) {
        LCU_ << "device:" << deviceID << "not known by this ContorlUnit";
        throw CException(-2, "Device not known by this control unit", "AbstractControlUnit::_init");
    }
    
    //check to see if the device can ben initialized
    if(deviceState != INIT_STATE) {
        LCU_ << "device:" << deviceID << " already initialized";
        throw CException(-3, "Device Already Initialized", "AbstractControlUnit::_init");
    }
    
    LCU_ << "Initialize CU Database for device:" << deviceID;
    DeviceSchemaDB::addAttributeToDataSetFromDataWrapper(*initConfiguration);
    
    LCU_ << "Initialize the DSAttribute handler engine for device:" << deviceID;
    utility::StartableService::initImplementation(attributeHandlerEngine, static_cast<void*>(initConfiguration), "DSAttribute handler engine", "AbstractControlUnit::_init");
    
    //initialize key data storage for device id
    LCU_ << "Create KeyDataStorage device:" << deviceID;
    tmpKDS = DataManager::getInstance()->getKeyDataStorageNewInstanceForKey(deviceID);
    
    LCU_ << "Call init implementation for deviceID:" << deviceID;
    tmpKDS->init(initConfiguration);
    
    keyDataStorage = tmpKDS;
    
    LCU_ << "Initializing slow command sandbox" << deviceID;
    utility::StartableService::initImplementation(slowCommandExecutor, static_cast<void*>(initConfiguration), "Slow Command Executor", "AbstractControlUnit::_init");
    //associate the data storage
    slowCommandExecutor->commandSandbox.keyDataStorage = keyDataStorage;
    
    LCU_ << "Start custom inititialization" << deviceID;
    //initializing the device in control unit
    init();
    
    //advance status
    deviceState++;
    
    //call update param function
    updateConfiguration(initConfiguration, detachParam);
    
    //reset run schedule heartbeat
    heartBeatIntervall = boost::chrono::seconds(0);
    
    deviceExplicitState = CUStateKey::INIT;
    return NULL;
}

/*
 deinit all datastorage
 */
CDataWrapper* AbstractControlUnit::_deinit(CDataWrapper *deinitParam, bool& detachParam) throw(CException) {
    recursive_mutex::scoped_lock  lock(managing_cu_mutex);
    LCU_ << "Deinitializating AbstractControlUnit";
    
    if(!deinitParam || !deinitParam->hasKey(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID)) {
        throw CException(-1, "No Device Defined in param", "AbstractControlUnit::_deinit");
    }
    
    string deviceID = deinitParam->getStringValue(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID);
    LCU_ << "Deinitialization Phase for device:" << deviceID;
    if(deviceID.compare(DeviceSchemaDB::getDeviceID())) {
        LCU_ << "device:" << deviceID << "not known by this ContorlUnit";
        throw CException(-2, "Deviuce not known by this control unit", "AbstractControlUnit::_deinit");
    }
    
    //check to see if the device can ben initialized
    if(deviceState != INIT_STATE+1) {
        LCU_ << "device:" << deviceID << " already initialized";
        throw CException(-3, "Device Not Initialized", "AbstractControlUnit::_deinit");
    }
    
    //deinit the control unit
    LCU_ << "Start custom deinitialization for device:" << deviceID;
    deinit();
    
    LCU_ << "Deinitialize sandbox deinitialization for device:" << deviceID;
    utility::StartableService::deinitImplementation(slowCommandExecutor, "Slow Command Executor", "AbstractControlUnit::_deinit");
    //deassociate the data storage
    slowCommandExecutor->commandSandbox.keyDataStorage = NULL;
    
    //remove key data storage
    if(keyDataStorage) {
        LCU_ << "Delete data storage driver for device:" << deviceID;
        keyDataStorage->deinit();
        delete(keyDataStorage);
        keyDataStorage = NULL;
    }
    
    LCU_ << "Deinitializing the DSAttribute handler engine for device:" << deviceID;
    utility::StartableService::deinitImplementation(attributeHandlerEngine, "DSAttribute handler engine", "AbstractControlUnit::_deinit");

    deviceState--;
    
    deviceExplicitState = CUStateKey::DEINIT;
    return NULL;
}

/*
 Starto the  Control Unit scheduling for device
 */
CDataWrapper* AbstractControlUnit::_start(CDataWrapper *startParam, bool& detachParam) throw(CException) {
    recursive_mutex::scoped_lock  lock(managing_cu_mutex);
    
    if(!startParam || !startParam->hasKey(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID)) {
        throw CException(-1, "No Device Defined in param", "AbstractControlUnit::_start");
    }
    
    string deviceID = startParam->getStringValue(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID);
    if(deviceID.compare(DeviceSchemaDB::getDeviceID())) {
        LCU_ << "device:" << deviceID << "not known by this ContorlUnit";
        throw CException(-2, "Deviuce not known by this control unit", "AbstractControlUnit::_start");
    }
    
    //check to see if the device can ben initialized
    if(deviceState != START_STATE) {
        LCU_ << "device:" << deviceID << " already strted";
        throw CException(-3, "Device already started", "AbstractControlUnit::_start");
    }
    
    LCU_ << "Start sandbox for device:" << deviceID;
    utility::StartableService::startImplementation(slowCommandExecutor, "Slow Command Executor", "AbstractControlUnit::_start");
    
    LCU_ << "Starting thread for device:" << deviceID;
    threadStartStopManagment(true);
    LCU_ << "Thread started for device:" << deviceID;
    
    deviceState++;
    
    deviceExplicitState = CUStateKey::START;
    return NULL;
}

/*
 Stop the Custom Control Unit scheduling for device
 */
CDataWrapper* AbstractControlUnit::_stop(CDataWrapper *stopParam, bool& detachParam) throw(CException) {
    recursive_mutex::scoped_lock  lock(managing_cu_mutex);
    if(!stopParam || !stopParam->hasKey(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID)) {
        throw CException(-1, "No Device Defined in param", "AbstractControlUnit::_stop");
    }
    string deviceID = stopParam->getStringValue(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID);
    if(deviceID.compare(DeviceSchemaDB::getDeviceID())) {
        LCU_ << "device:" << deviceID << "not known by this ContorlUnit";
        throw CException(-2, "Device not known by this control unit", "AbstractControlUnit::_stop");
    }
    
    //check to see if the device can ben initialized
    if(deviceState != START_STATE+1) {
        LCU_ << "device:" << deviceID << " not started";
        throw CException(-3, "Device not startded", "AbstractControlUnit::_stop");
    }
    
    //call custom stop method
    stop();
    
    
    LCU_ << "Stop slow command executor for device:" << deviceID;
    utility::StartableService::stopImplementation(slowCommandExecutor, "Slow Command Executor", "AbstractControlUnit::_stop");
    
    //manage the thread
    threadStartStopManagment(false);
    
    //set device state
    deviceState--;
    
    //set the explicit state
    deviceExplicitState = CUStateKey::STOP;
    
    return NULL;
}

/*
 Receive the evento for set the dataset input element
 */
CDataWrapper* AbstractControlUnit::_setDatasetAttribute(CDataWrapper *datasetAttributeValues,  bool& detachParam) throw (CException) {
    CDataWrapper *executionResult = NULL;
    //lock shared access to control unit
    recursive_mutex::scoped_lock  lock(managing_cu_mutex);
    
    //load all keyDataStorageMap for the registered devices
    if(!datasetAttributeValues || !datasetAttributeValues->hasKey(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID)) {
        throw CException(-1, "No Device Defined in param", "AbstractControlUnit::_setDatasetAttribute");
    }
    //retrive the deviceid
    string deviceID = datasetAttributeValues->getStringValue(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID);
    if(deviceExplicitState == CUStateKey::DEINIT) {
        throw CException(-2, "The Control Unit is in deinit state", "AbstractControlUnit::_setDatasetAttribute");
    }
    try {
        //send dataset attribute change pack to control unit implementation
        executionResult = setDatasetAttribute(datasetAttributeValues, detachParam);
        
        //serach and execute handler
        attributeHandlerEngine->executeHandler(deviceID, datasetAttributeValues);
        
        //at this time notify the wel gone setting of comand
        if(deviceEventChannel) deviceEventChannel->notifyForAttributeSetting(deviceID.c_str(), 0);
        
    } catch (CException& ex) {
        //at this time notify the wel gone setting of comand
        if(deviceEventChannel) deviceEventChannel->notifyForAttributeSetting(deviceID.c_str(), ex.errorCode);
        
        throw ex;
    }
    
    return executionResult;
}

/*
 Get the current control unit state
 */
CDataWrapper* AbstractControlUnit::_getState(CDataWrapper* getStatedParam, bool& detachParam) throw(CException) {
    if(!getStatedParam->hasKey(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID)){
        throw CException(-1, "Get State Pack without DeviceID", "AbstractControlUnit::getState");
    }
    CDataWrapper *stateResult = new CDataWrapper();
    string deviceID = getStatedParam->getStringValue(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID);
    stateResult->addInt32Value(CUStateKey::CONTROL_UNIT_STATE, deviceExplicitState);
    return stateResult;
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
    
    if(deviceID.compare(DeviceSchemaDB::getDeviceID())) {
        LCU_ << "device:" << deviceID << "not known by this ContorlUnit";
        throw CException(-2, "Device not known by this control unit", "AbstractControlUnit::_stop");
    }
    
    //check to see if the device can ben initialized
    if(deviceState == INIT_STATE) {
        LCU_ << "device:" << deviceID << " not initialized";
        throw CException(-3, "Device Not Initilized", "AbstractControlUnit::updateConfiguration");
    }
    
    //check to see if the device can ben initialized
    if(keyDataStorage) {
        keyDataStorage->updateConfiguration(updatePack);
    }
    
    if(updatePack->hasKey(CUDefinitionKey::CS_CM_THREAD_SCHEDULE_DELAY)){
        //we need to configure the delay  from a run() call and the next
        uint64_t uSecdelay = updatePack->getUInt64Value(CUDefinitionKey::CS_CM_THREAD_SCHEDULE_DELAY);
        //check if we need to update the scehdule time
        if(uSecdelay != schedulerThread->getDelayBeetwenTask()){
            LCU_ << "Update schedule delay in:" << uSecdelay << " microsecond";
            schedulerThread->setDelayBeetwenTask(uSecdelay);
            //send envet for the update
            //----------------------
            // we need to optimize and be sure that event channel
            // is mandatory so we can left over the 'if' check
            //----------------------
            if(deviceEventChannel) deviceEventChannel->notifyForScheduleUpdateWithNewValue(deviceID.c_str(), uSecdelay);
        }
    }
    return NULL;
}


/*
 Execute the scehduling for the device
 */
void AbstractControlUnit::executeOnThread() throw(CException) {
    
    run();
    //check if we are in sequential or in threaded mode
    
    lastAcquiredTime = boost::chrono::duration_cast<boost::chrono::seconds>(boost::chrono::steady_clock::now().time_since_epoch());
    //check if we need to sendthe heartbeat
    if(deviceEventChannel && ((lastAcquiredTime - heartBeatIntervall) > boost::chrono::seconds(60))){
        //deviceEventChannel->notifyHeartbeat(deviceIDToSchedule.c_str());
        heartBeatIntervall = lastAcquiredTime;
    }
}

#pragma mark protected API


/*
 Send device data to output buffer
 */
void AbstractControlUnit::pushDataSet(CDataWrapper *acquiredData) {
    //send data to related buffer
    keyDataStorage->pushDataSet(acquiredData);
}

/*
 get last dataset for a specified key
 */
ArrayPointer<CDataWrapper> *AbstractControlUnit::getLastDataSet() {
    //use keydatastorage from map
    return keyDataStorage->getLastDataSet();
}

/*
 Return a new instance of CDataWrapper filled with a mandatory data
 according to key
 */
CDataWrapper *AbstractControlUnit::getNewDataWrapper() {
    return keyDataStorage->getNewDataWrapper();
}

//! Perform a command registration
/*!
 An instance of the command si registered within the executor.
 */
void AbstractControlUnit::setDefaultCommand(const char * dafaultCommandName) {
    CHAOS_ASSERT(slowCommandExecutor)
    slowCommandExecutor->setDefaultCommand(dafaultCommandName);
}

