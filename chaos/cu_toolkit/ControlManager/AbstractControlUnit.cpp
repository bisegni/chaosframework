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

using namespace boost::uuids;

using namespace chaos::common::data;

using namespace chaos::cu;
using namespace chaos::cu::driver_manager::driver;

#define ACULAPP_ LAPP_ << "[Control Unit:"<<control_unit_instance<<"-"<<control_unit_id<<"] - "
#define ACULDBG_ LDBG_ << "[Control Unit:"<<control_unit_instance<<"-"<<control_unit_id<<"] - "
#define ACULERR_ LERR_ << "[Control Unit:"<<control_unit_instance<<"-"<<control_unit_id<<"]("<<__LINE__<<") - "

//! Contructor with type and id
AbstractControlUnit::AbstractControlUnit(const std::string& _control_unit_type,
										 const std::string& _control_unit_id,
										 const std::string& _control_unit_param):
DatasetDB(GlobalConfiguration::getInstance()->getOption<bool>(CU_OPT_IN_MEMORY_DATABASE)),
control_unit_instance(UUIDUtil::generateUUIDLite()),
control_unit_type(_control_unit_type),
control_unit_id(_control_unit_id),
control_unit_param(_control_unit_param) {
}

//! Contructor with driver
AbstractControlUnit::AbstractControlUnit(const std::string& _control_unit_type,
										 const std::string& _control_unit_id,
										 const std::string& _control_unit_param,
										 const ControlUnitDriverList& _control_unit_drivers):
DatasetDB(GlobalConfiguration::getInstance()->getOption<bool>(CU_OPT_IN_MEMORY_DATABASE)),
control_unit_instance(UUIDUtil::generateUUIDLite()),
control_unit_type(_control_unit_type),
control_unit_id(_control_unit_id),
control_unit_param(_control_unit_param) {
	//copy array
	for (int idx = 0; idx < _control_unit_drivers.size(); idx++){
		control_unit_drivers.push_back(_control_unit_drivers[idx]);
	}
}

/*!
 Destructor a new CU with an identifier
 */
AbstractControlUnit::~AbstractControlUnit() {
}

/*
 return the CU name
 */
const char * AbstractControlUnit::getCUInstance(){
    return control_unit_instance.c_str();
};

const char * AbstractControlUnit::getCUID() {
	return control_unit_id.c_str();
}

/*
 Add a new KeyDataStorage for a specific key
 */
void AbstractControlUnit::setKeyDataStorage(KeyDataStorage* _keyDatStorage) {
    keyDataStorage = _keyDatStorage;
}

/*
 fill the CDataWrapper with AbstractCU system configuration, this method
 is called after getStartConfiguration directly by sandbox. in this method
 are defined the action for the input element of the dataset
 */
void AbstractControlUnit::_defineActionAndDataset(CDataWrapper& setupConfiguration)  throw(CException) {
    vector<std::string> tempStringVector;
    
	if(control_unit_id.size()) {
		setDeviceID(control_unit_id);
	}
	
    //add the CU isntance, this can be redefinide by user in the unitDefineActionAndDataset method
    //for let the CU have the same instance at every run
    setupConfiguration.addStringValue(CUDefinitionKey::CS_CM_CU_INSTANCE, control_unit_instance);
    //check if as been setuped a file for configuration
    //LCU_ << "Check if as been setup a json file path to configura CU:" << CU_IDENTIFIER_C_STREAM;
    //loadCDataWrapperForJsonFile(setupConfiguration);
    
    //first call the setup abstract method used by the implementing CU to define action, dataset and other
    //usefull value
    ACULAPP_ << "Define Actions and Dataset";
    unitDefineActionAndDataset();
    
    //for now we need only to add custom action for expose to rpc
    //input element of the dataset
    ACULAPP_ << "Define the base action for map the input attribute of the dataset";
    AbstActionDescShrPtr
    actionDescription = addActionDescritionInstance<AbstractControlUnit>(this,
                                                                         &AbstractControlUnit::_setDatasetAttribute,
                                                                         "setDatasetAttribute",
                                                                         "method for set the input element for the dataset");
    
    //expose updateConfiguration Methdo to rpc
    ACULAPP_ << "Register updateConfiguration action";
    addActionDescritionInstance<AbstractControlUnit>(this,
                                                     &AbstractControlUnit::updateConfiguration,
                                                     "updateConfiguration",
                                                     "Update control unit configuration");
    
    ACULAPP_ << "Register initDevice action";
    addActionDescritionInstance<AbstractControlUnit>(this,
                                                     &AbstractControlUnit::_init,
                                                     ChaosSystemDomainAndActionLabel::ACTION_CU_INIT,
                                                     "Perform the control unit initialization");
    
    ACULAPP_ << "Register deinitDevice action";
    addActionDescritionInstance<AbstractControlUnit>(this,
                                                     &AbstractControlUnit::_deinit,
                                                     ChaosSystemDomainAndActionLabel::ACTION_CU_DEINIT,
                                                     "Perform the control unit deinitialization");
    ACULAPP_ << "Register startDevice action";
    addActionDescritionInstance<AbstractControlUnit>(this,
                                                     &AbstractControlUnit::_start,
                                                     ChaosSystemDomainAndActionLabel::ACTION_CU_START,
                                                     "Start the control unit scheduling");
    
    ACULAPP_ << "Register stopDevice action";
    addActionDescritionInstance<AbstractControlUnit>(this,
                                                     &AbstractControlUnit::_stop,
                                                     ChaosSystemDomainAndActionLabel::ACTION_CU_STOP,
                                                     "Stop the control unit scheduling");
    ACULAPP_ << "Register getState action";
    addActionDescritionInstance<AbstractControlUnit>(this,
                                                     &AbstractControlUnit::_getState,
                                                     ChaosSystemDomainAndActionLabel::ACTION_CU_GET_STATE,
                                                     "Get the state of the running control unit");
	ACULAPP_ << "Register getInfo action";
    addActionDescritionInstance<AbstractControlUnit>(this,
                                                     &AbstractControlUnit::_getInfo,
                                                     ChaosSystemDomainAndActionLabel::ACTION_CU_GET_INFO,
                                                     "Get the information about running control unit");
	
    ACULAPP_ << "Get Description";
    //grab dataset description
    DatasetDB::fillDataWrapperWithDataSetDescription(setupConfiguration);
    
#if DEBUG
    ACULAPP_ << setupConfiguration.getJSONString();
#endif
}

void AbstractControlUnit::unitDefineDriver(std::vector<cu_driver::DrvRequestInfo>& neededDriver) {
	
	for(ControlUnitDriverListIterator iter = control_unit_drivers.begin();
		iter != control_unit_drivers.end();
		iter++) {
		//copy driver info to the system array reference
		neededDriver.push_back(*iter);
	}
}

/*
 Define the control unit DataSet and Action into
 a CDataWrapper
 */
void AbstractControlUnit::_undefineActionAndDataset() throw(CException) {
    ACULAPP_ << "Remove Action Description";
    //register command manager action
    //CommandManager::getInstance()->deregisterAction(this);
    
}

//! Get all managem declare action instance
void AbstractControlUnit::_getDeclareActionInstance(std::vector<const chaos::DeclareAction *>& declareActionInstance) {
    declareActionInstance.push_back(this);
}
//----------------------------------------- protected initi/deinit method ------------------------------------------------
/*
 Initialize the Custom Contro Unit and return the configuration
 */
CDataWrapper* AbstractControlUnit::_init(CDataWrapper *initConfiguration, bool& detachParam) throw(CException) {
	utility::StartableService::initImplementation(this, static_cast<void*>(initConfiguration), "AbstractControlUnit", "AbstractControlUnit::_init");
	
	//the init of the implementation unit goes after the infrastructure one
	ACULAPP_ << "Start custom inititialization";
	unitInit();
	
	//call update param function
	updateConfiguration(initConfiguration, detachParam);
	
    return NULL;
}

/*
 Starto the  Control Unit scheduling for device
 */
CDataWrapper* AbstractControlUnit::_start(CDataWrapper *startParam, bool& detachParam) throw(CException) {
    //call start method of the startable interface
	utility::StartableService::startImplementation(this, "AbstractControlUnit", "AbstractControlUnit::_start");
	ACULAPP_ << "Start sublass for deviceID:" << DatasetDB::getDeviceID();
	unitStart();
    return NULL;
}

/*
 Stop the Custom Control Unit scheduling for device
 */
CDataWrapper* AbstractControlUnit::_stop(CDataWrapper *stopParam, bool& detachParam) throw(CException) {
    //call start method of the startable interface
    utility::StartableService::stopImplementation(this, "AbstractControlUnit", "AbstractControlUnit::_stop");
	//first we need to stop the implementation unit
    ACULAPP_ << "Stop sublass for deviceID:" << DatasetDB::getDeviceID();
    unitStop();
    return NULL;
}

/*
 deinit all datastorage
 */
CDataWrapper* AbstractControlUnit::_deinit(CDataWrapper *deinitParam, bool& detachParam) throw(CException) {
    //call deinit method of the startable interface
    utility::StartableService::deinitImplementation(this, "AbstractControlUnit", "AbstractControlUnit::_deinit");
	
	//first we start the deinitializaiton of the implementation unit
    ACULAPP_ << "Deinit custom deinitialization for device:" << DatasetDB::getDeviceID();
    unitDeinit();
	
    //remove key data storage
    if(keyDataStorage) {
        ACULAPP_ << "Delete data storage driver for device:" << DatasetDB::getDeviceID();
        keyDataStorage->deinit();
        delete(keyDataStorage);
        keyDataStorage = NULL;
    }
	
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
CDataWrapper* AbstractControlUnit::_setDatasetAttribute(CDataWrapper *datasetAttributeValues,  bool& detachParam) throw (CException) {
    CDataWrapper *executionResult = NULL;
    try {
        if(!datasetAttributeValues) {
            throw CException(-1, "No Input parameter", "AbstractControlUnit::_setDatasetAttribute");
        }
        
#if DEBUG
        ACULAPP_ << datasetAttributeValues->getJSONString();
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
        if(deviceEventChannel) deviceEventChannel->notifyForAttributeSetting(DatasetDB::getDeviceID(), ex.errorCode);
        
        throw ex;
    }
    
    return executionResult;
}

// Startable Service method
void AbstractControlUnit::init(void *initData) throw(CException) {
	CDataWrapper *initConfiguration = static_cast<CDataWrapper*>(initData);
	if(!initConfiguration ||
	   !initConfiguration->hasKey(DatasetDefinitionkey::DEVICE_ID)) {
		throw CException(-1, "No Device Init information in param", "AbstractControlUnit::_init");
	}
	
	std::string deviceID = initConfiguration->getStringValue(DatasetDefinitionkey::DEVICE_ID);
	if(deviceID.compare(DatasetDB::getDeviceID())) {
		ACULAPP_ << "device:" << deviceID << "not known by this ContorlUnit";
		throw CException(-2, "Device not known by this control unit", "AbstractControlUnit::_init");
	}
	ACULAPP_ << "Initializating Phase for device:" << deviceID;
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
	
    //cast to the CDatawrapper instance
    
    ACULAPP_ << "Initialize CU Database for device:" << deviceID;
    DatasetDB::addAttributeToDataSetFromDataWrapper(*initConfiguration);
    
    //initialize key data storage for device id
    ACULAPP_ << "Create KeyDataStorage device:" << deviceID;
    keyDataStorage = DataManager::getInstance()->getKeyDataStorageNewInstanceForKey(deviceID);
    
    ACULAPP_ << "Call KeyDataStorage init implementation for deviceID:" << deviceID;
    keyDataStorage->init(initConfiguration);
}

// Startable Service method
void AbstractControlUnit::start() throw(CException) {
}

// Startable Service method
void AbstractControlUnit::stop() throw(CException) {
}

// Startable Service method
void AbstractControlUnit::deinit() throw(CException) {
}

/*
 Get the current control unit state
 */
CDataWrapper* AbstractControlUnit::_getState(CDataWrapper* getStatedParam, bool& detachParam) throw(CException) {
    if(!getStatedParam->hasKey(DatasetDefinitionkey::DEVICE_ID)){
        throw CException(-1, "Get State Pack without DeviceID", "AbstractControlUnit::_getState");
    }
    CDataWrapper *stateResult = new CDataWrapper();
    string deviceID = getStatedParam->getStringValue(DatasetDefinitionkey::DEVICE_ID);
    
    stateResult->addInt32Value(CUStateKey::CONTROL_UNIT_STATE, static_cast<CUStateKey::ControlUnitState>(utility::StartableService::getServiceState()));
    return stateResult;
}

/*
 Get the current control unit state
 */
CDataWrapper* AbstractControlUnit::_getInfo(CDataWrapper* getStatedParam, bool& detachParam) throw(CException) {
    CDataWrapper *stateResult = new CDataWrapper();
    //set the string representing the type of the control unit
    stateResult->addStringValue(CUDefinitionKey::CS_CM_CU_TYPE, control_unit_type);
    return stateResult;
}

/*
 Update the configuration for all descendand tree in the Control Uniti class struccture
 */
CDataWrapper*  AbstractControlUnit::updateConfiguration(CDataWrapper* updatePack, bool& detachParam) throw (CException) {
    //load all keyDataStorageMap for the registered devices
    if(!updatePack || !updatePack->hasKey(DatasetDefinitionkey::DEVICE_ID)) {
        throw CException(-1, "Update pack without DeviceID", "AbstractControlUnit::updateConfiguration");
    }
    
    string deviceID = updatePack->getStringValue(DatasetDefinitionkey::DEVICE_ID);
    
    if(deviceID.compare(DatasetDB::getDeviceID())) {
        ACULAPP_ << "device:" << DatasetDB::getDeviceID() << "not known by this ContorlUnit";
        throw CException(-2, "Device not known by this control unit", "AbstractControlUnit::_stop");
    }
    
    //check to see if the device can ben initialized
    if(utility::StartableService::getServiceState() == INIT_STATE) {
        ACULAPP_ << "device:" << DatasetDB::getDeviceID() << " not initialized";
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
cu_driver::DriverAccessor *AbstractControlUnit::getAccessoInstanceByIndex(int idx) {
	if( idx >= accessorInstances.size() ) return NULL;
	return accessorInstances[idx];
}
