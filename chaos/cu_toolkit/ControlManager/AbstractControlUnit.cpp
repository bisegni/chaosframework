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
#include <chaos/cu_toolkit/driver_manager/DriverManager.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <boost/lexical_cast.hpp>

namespace cu_driver = chaos::cu::driver_manager::driver;
namespace cu = chaos::cu;
namespace uuid = boost::uuids;

#define LCU_ LAPP_ << "[Control Unit:"<<getCUInstance()<<"] - "


cu::AbstractControlUnit::AbstractControlUnit():DeviceSchemaDB(false), cuInstance(UUIDUtil::generateUUIDLite()) {
}

/*!
 Destructor a new CU with an identifier
 */
cu::AbstractControlUnit::~AbstractControlUnit() {
}

/*
 return the CU name
 */
const char * cu::AbstractControlUnit::getCUInstance(){
    return cuInstance.size()?cuInstance.c_str():"Instance no yet configured";
};

/*
 Add a new KeyDataStorage for a specific key
 */
void cu::AbstractControlUnit::setKeyDataStorage(KeyDataStorage* _keyDatStorage) {
    keyDataStorage = _keyDatStorage;
}

/*
 fill the CDataWrapper with AbstractCU system configuration, this method
 is called after getStartConfiguration directly by sandbox. in this method
 are defined the action for the input element of the dataset
 */
void cu::AbstractControlUnit::_defineActionAndDataset(CDataWrapper& setupConfiguration)  throw(CException) {
    vector<std::string> tempStringVector;
    
    //add the CU isntance, this can be redefinide by user in the unitDefineActionAndDataset method
    //for let the CU have the same instance at every run
    setupConfiguration.addStringValue(CUDefinitionKey::CS_CM_CU_INSTANCE, cuInstance);
    //check if as been setuped a file for configuration
    //LCU_ << "Check if as been setup a json file path to configura CU:" << CU_IDENTIFIER_C_STREAM;
    //loadCDataWrapperForJsonFile(setupConfiguration);
    
    //first call the setup abstract method used by the implementing CU to define action, dataset and other
    //usefull value
    LCU_ << "Define Actions and Dataset for:" << CU_IDENTIFIER_C_STREAM;
    unitDefineActionAndDataset();
    
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
    addActionDescritionInstance<AbstractControlUnit>(this,
                                                     &AbstractControlUnit::updateConfiguration,
                                                     "updateConfiguration",
                                                     "Update Configuration");
    
    LCU_ << "Register initDevice action";
    addActionDescritionInstance<AbstractControlUnit>(this,
                                                     &AbstractControlUnit::_init,
                                                     ChaosSystemDomainAndActionLabel::ACTION_DEVICE_INIT,
                                                     "Perform the device initialization");
    
    LCU_ << "Register deinitDevice action";
    addActionDescritionInstance<AbstractControlUnit>(this,
                                                     &AbstractControlUnit::_deinit,
                                                     ChaosSystemDomainAndActionLabel::ACTION_DEVICE_DEINIT,
                                                     "Perform the device deinitialization");
    LCU_ << "Register startDevice action";
    addActionDescritionInstance<AbstractControlUnit>(this,
                                                     &AbstractControlUnit::_start,
                                                     ChaosSystemDomainAndActionLabel::ACTION_DEVICE_START,
                                                     "Sart the device scheduling");
    
    LCU_ << "Register stopDevice action";
    addActionDescritionInstance<AbstractControlUnit>(this,
                                                     &AbstractControlUnit::_stop,
                                                     ChaosSystemDomainAndActionLabel::ACTION_DEVICE_STOP,
                                                     "Stop the device scheduling");
    LCU_ << "Register getState action";
    addActionDescritionInstance<AbstractControlUnit>(this,
                                                     &AbstractControlUnit::_getState,
                                                     ChaosSystemDomainAndActionLabel::ACTION_DEVICE_GET_STATE,
                                                     "Get the state of the device");
    
    LCU_ << "Get Description for Control Unit:" << CU_IDENTIFIER_C_STREAM;
    //grab dataset description
    DeviceSchemaDB::fillDataWrapperWithDataSetDescription(setupConfiguration);
    
#if DEBUG
    LCU_ << setupConfiguration.getJSONString();
#endif
}

/*
 Define the control unit DataSet and Action into
 a CDataWrapper
 */
void cu::AbstractControlUnit::_undefineActionAndDataset() throw(CException) {
    LCU_ << "Remove Action Description for Control Unit:" << CU_IDENTIFIER_C_STREAM;
    //register command manager action
    //CommandManager::getInstance()->deregisterAction(this);
    
}

//! Get all managem declare action instance
void cu::AbstractControlUnit::_getDeclareActionInstance(std::vector<const chaos::DeclareAction *>& declareActionInstance) {
    declareActionInstance.push_back(this);
}
//----------------------------------------- protected initi/deinit method ------------------------------------------------
/*
 Initialize the Custom Contro Unit and return the configuration
 */
chaos::CDataWrapper* cu::AbstractControlUnit::_init(chaos::CDataWrapper *initConfiguration, bool& detachParam) throw(CException) {
    
    if(!initConfiguration ||
       !initConfiguration->hasKey(DatasetDefinitionkey::DEVICE_ID)) {
        throw CException(-1, "No Device Init information in param", "AbstractControlUnit::_init");
    }
    
    std::string deviceID = initConfiguration->getStringValue(DatasetDefinitionkey::DEVICE_ID);
    if(deviceID.compare(DeviceSchemaDB::getDeviceID())) {
        LCU_ << "device:" << deviceID << "not known by this ContorlUnit";
        throw CException(-2, "Device not known by this control unit", "AbstractControlUnit::_init");
    }

	//at this point and before the unit implementation init i need to get
	//the infromation about the needed drivers
	std::vector<cu_driver::DrvRequestInfo> unitNeededDrivers;
	
	//got the needded driver definition
	unitDefineDriver(unitNeededDrivers);
	
	accessorInstances.clear();
	
	for (int idx = 0;
		 idx != unitNeededDrivers.size();
		 idx++) {
		driver_manager::driver::DriverAccessor *accessorInstance = driver_manager::DriverManager::getInstance()->getNewAccessorForDriverInstance(unitNeededDrivers[idx]);
		accessorInstances.push_back(accessorInstance);
	}
	
    //call init method of the startable interface
    utility::StartableService::initImplementation(this, static_cast<void*>(initConfiguration), "AbstractControlUnit", "AbstractControlUnit::_init");
	
	//the init of the implementation unit goes after the infrastructure one
    LCU_ << "Start custom inititialization" << deviceID;
    unitInit();
    
    //call update param function
    updateConfiguration(initConfiguration, detachParam);
    return NULL;
}

/*
 Starto the  Control Unit scheduling for device
 */
chaos::CDataWrapper* cu::AbstractControlUnit::_start(chaos::CDataWrapper *startParam, bool& detachParam) throw(CException) {
    if(!startParam ||
       !startParam->hasKey(DatasetDefinitionkey::DEVICE_ID)) {
        throw CException(-1, "No Device Defined in param", "AbstractControlUnit::_start");
    }
    
    string deviceID = startParam->getStringValue(DatasetDefinitionkey::DEVICE_ID);
    if(deviceID.compare(cu::DeviceSchemaDB::getDeviceID())) {
        LCU_ << "device:" << deviceID << "not known by this ContorlUnit";
        throw CException(-2, "Deviuce not known by this control unit", "AbstractControlUnit::_start");
    }
    
    //call start method of the startable interface
    utility::StartableService::startImplementation(this, "AbstractControlUnit", "AbstractControlUnit::_start");
	
	//the start of the implementation unit goes after the infrastructure one
    LCU_ << "Start sublass for deviceID:" << cu::DeviceSchemaDB::getDeviceID();
    unitStart();
    return NULL;
}

/*
 Stop the Custom Control Unit scheduling for device
 */
chaos::CDataWrapper* cu::AbstractControlUnit::_stop(chaos::CDataWrapper *stopParam, bool& detachParam) throw(CException) {
    if(!stopParam ||
       !stopParam->hasKey(DatasetDefinitionkey::DEVICE_ID)) {
        throw CException(-1, "No Device Defined in param", "AbstractControlUnit::_stop");
    }
    string deviceID = stopParam->getStringValue(DatasetDefinitionkey::DEVICE_ID);
    if(deviceID.compare(cu::DeviceSchemaDB::getDeviceID())) {
        LCU_ << "device:" << deviceID << "not known by this ContorlUnit";
        throw CException(-2, "Device not known by this control unit", "AbstractControlUnit::_stop");
    }
    
	//first we need to stop the implementation unit
    LCU_ << "Stop sublass for deviceID:" << cu::DeviceSchemaDB::getDeviceID();
    unitStop();
	
    //call start method of the startable interface
    utility::StartableService::stopImplementation(this, "AbstractControlUnit", "AbstractControlUnit::_stop");

    return NULL;
}

/*
 deinit all datastorage
 */
chaos::CDataWrapper* cu::AbstractControlUnit::_deinit(CDataWrapper *deinitParam, bool& detachParam) throw(CException) {
    LCU_ << "Deinitializating AbstractControlUnit";
    
    if(!deinitParam ||
       !deinitParam->hasKey(DatasetDefinitionkey::DEVICE_ID)) {
        throw CException(-1, "No Device Defined in param", "AbstractControlUnit::_deinit");
    }
    
    string deviceID = deinitParam->getStringValue(DatasetDefinitionkey::DEVICE_ID);
    if(deviceID.compare(cu::DeviceSchemaDB::getDeviceID())) {
        LCU_ << "device:" << cu::DeviceSchemaDB::getDeviceID() << "not known by this ContorlUnit";
        throw CException(-2, "Deviuce not known by this control unit", "AbstractControlUnit::_deinit");
    }
    
	
    //first we start the deinitializaiton of the implementation unit
    LCU_ << "Deinit custom deinitialization for device:" << cu::DeviceSchemaDB::getDeviceID();
    unitDeinit();
	
    //call deinit method of the startable interface
    utility::StartableService::deinitImplementation(this, "AbstractControlUnit", "AbstractControlUnit::_deinit");
	
	//clear the accessor of the driver
	for (int idx = 0;
		 idx != accessorInstances.size();
		 idx++) {
		driver_manager::DriverManager::getInstance()->releaseAccessor(accessorInstances[idx]);
	}
	//clear the vector
	accessorInstances.clear();
    return NULL;
}

/*
 Receive the evento for set the dataset input element
 */
chaos::CDataWrapper* cu::AbstractControlUnit::_setDatasetAttribute(chaos::CDataWrapper *datasetAttributeValues,  bool& detachParam) throw (CException) {
    CDataWrapper *executionResult = NULL;
    try {
        if(!datasetAttributeValues) {
            throw CException(-1, "No Input parameter", "AbstractControlUnit::_setDatasetAttribute");
        }
        
#if DEBUG
        LCU_ << datasetAttributeValues->getJSONString();
#endif
        
        if(!datasetAttributeValues->hasKey(DatasetDefinitionkey::DEVICE_ID)) {
            throw CException(-2, "No Device Defined in param", "AbstractControlUnit::setDatasetAttribute");
        }
        //retrive the deviceid
        string deviceID = datasetAttributeValues->getStringValue(DatasetDefinitionkey::DEVICE_ID);
        if(utility::StartableService::getServiceState() == CUStateKey::DEINIT) {
            throw CException(-3, "The Control Unit is in deinit state", "AbstractControlUnit::_setDatasetAttribute");
        }
        //send dataset attribute change pack to control unit implementation
        executionResult = setDatasetAttribute(datasetAttributeValues, detachParam);
        
    } catch (CException& ex) {
        //at this time notify the wel gone setting of comand
        if(deviceEventChannel) deviceEventChannel->notifyForAttributeSetting(cu::DeviceSchemaDB::getDeviceID(), ex.errorCode);
        
        throw ex;
    }
    
    return executionResult;
}

// Startable Service method
void cu::AbstractControlUnit::init(void *initData) throw(CException) {
    //cast to the CDatawrapper instance
    std::string deviceID = cu::DeviceSchemaDB::getDeviceID();
    CDataWrapper *initConfiguration = static_cast<CDataWrapper*>(initData);
	LCU_ << "Initializating Phase for device:" << deviceID;

    
    LCU_ << "Initialize CU Database for device:" << deviceID;
    DeviceSchemaDB::addAttributeToDataSetFromDataWrapper(*initConfiguration);
    
    //initialize key data storage for device id
    LCU_ << "Create KeyDataStorage device:" << deviceID;
    keyDataStorage = DataManager::getInstance()->getKeyDataStorageNewInstanceForKey(deviceID);
    
    LCU_ << "Call KeyDataStorage init implementation for deviceID:" << deviceID;
    keyDataStorage->init(initConfiguration);
}

// Startable Service method
void cu::AbstractControlUnit::start() throw(CException) {
    LCU_ << "Start Phase for device:" << cu::DeviceSchemaDB::getDeviceID();

}

// Startable Service method
void cu::AbstractControlUnit::stop() throw(CException) {
    LCU_ << "Stop Phase for device:" << cu::DeviceSchemaDB::getDeviceID();
}

// Startable Service method
void cu::AbstractControlUnit::deinit() throw(CException) {
    LCU_ << "Deinitialization Phase for device:" << cu::DeviceSchemaDB::getDeviceID();
    
    //remove key data storage
    if(keyDataStorage) {
        LCU_ << "Delete data storage driver for device:" << cu::DeviceSchemaDB::getDeviceID();
        keyDataStorage->deinit();
        delete(keyDataStorage);
        keyDataStorage = NULL;
    }
}

/*
 Get the current control unit state
 */
chaos::CDataWrapper* cu::AbstractControlUnit::_getState(CDataWrapper* getStatedParam, bool& detachParam) throw(CException) {
    if(!getStatedParam->hasKey(DatasetDefinitionkey::DEVICE_ID)){
        throw CException(-1, "Get State Pack without DeviceID", "AbstractControlUnit::getState");
    }
    CDataWrapper *stateResult = new CDataWrapper();
    string deviceID = getStatedParam->getStringValue(DatasetDefinitionkey::DEVICE_ID);
    
    stateResult->addInt32Value(CUStateKey::CONTROL_UNIT_STATE, static_cast<CUStateKey::ControlUnitState>(utility::StartableService::getServiceState()));
    return stateResult;
}

/*
 Update the configuration for all descendand tree in the Control Uniti class struccture
 */
chaos::CDataWrapper*  cu::AbstractControlUnit::updateConfiguration(chaos::CDataWrapper* updatePack, bool& detachParam) throw (CException) {
    //load all keyDataStorageMap for the registered devices
    if(!updatePack || !updatePack->hasKey(DatasetDefinitionkey::DEVICE_ID)) {
        throw CException(-1, "Update pack without DeviceID", "AbstractControlUnit::updateConfiguration");
    }
    
    string deviceID = updatePack->getStringValue(DatasetDefinitionkey::DEVICE_ID);
    
    if(deviceID.compare(cu::DeviceSchemaDB::getDeviceID())) {
        LCU_ << "device:" << DeviceSchemaDB::getDeviceID() << "not known by this ContorlUnit";
        throw CException(-2, "Device not known by this control unit", "AbstractControlUnit::_stop");
    }
    
    //check to see if the device can ben initialized
    if(utility::StartableService::getServiceState() == INIT_STATE) {
        LCU_ << "device:" << cu::DeviceSchemaDB::getDeviceID() << " not initialized";
        throw CException(-3, "Device Not Initilized", "AbstractControlUnit::updateConfiguration");
    }
    
    //check to see if the device can ben initialized
    if(keyDataStorage) {
        keyDataStorage->updateConfiguration(updatePack);
    }
    
    return NULL;
}

//! return the accessor by an index
/*
 The index parameter correspond to the order that the driver infromation are
 added by the unit implementation into the function AbstractControlUnit::unitDefineDriver.
 */
cu_driver::DriverAccessor *cu::AbstractControlUnit::getAccessoInstanceByIndex(int idx) {
	if( idx >= accessorInstances.size() ) return NULL;
	return accessorInstances[idx];
}
