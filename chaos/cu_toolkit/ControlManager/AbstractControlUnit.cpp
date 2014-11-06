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
#include <boost/format.hpp>

using namespace boost::uuids;

using namespace chaos::common::data;
using namespace chaos::common::data::cache;

using namespace chaos::cu::data_manager;
using namespace chaos::cu::control_manager;
using namespace chaos::cu::driver_manager;
using namespace chaos::cu::driver_manager::driver;

#define ACULAPP_ LAPP_ << "[Control Unit:"<<control_unit_instance<<"-"<<control_unit_id<<"] - "
#define ACULDBG_ LDBG_ << "[Control Unit:"<<control_unit_instance<<"-"<<control_unit_id<<"] - "
#define ACULERR_ LERR_ << "[Control Unit:"<<control_unit_instance<<"-"<<control_unit_id<<"]("<<__LINE__<<") - "

//! Contructor with type and id
AbstractControlUnit::AbstractControlUnit(const std::string& _control_unit_type,
										 const std::string& _control_unit_id,
										 const std::string& _control_unit_param):
DatasetDB(GlobalConfiguration::getInstance()->getOption<bool>(CU_OPT_IN_MEMORY_DATABASE)),
control_key("none"),
control_unit_instance(UUIDUtil::generateUUIDLite()),
control_unit_type(_control_unit_type),
control_unit_id(_control_unit_id),
control_unit_param(_control_unit_param),
attribute_value_shared_cache(NULL),
attribute_shared_cache_wrapper(NULL),
acq_timestamp_cache_index(0),
device_event_channel(NULL){
}

//! Contructor with driver
AbstractControlUnit::AbstractControlUnit(const std::string& _control_unit_type,
										 const std::string& _control_unit_id,
										 const std::string& _control_unit_param,
										 const ControlUnitDriverList& _control_unit_drivers):
DatasetDB(GlobalConfiguration::getInstance()->getOption<bool>(CU_OPT_IN_MEMORY_DATABASE)),
control_key("none"),
control_unit_instance(UUIDUtil::generateUUIDLite()),
control_unit_type(_control_unit_type),
control_unit_id(_control_unit_id),
control_unit_param(_control_unit_param),
attribute_value_shared_cache(NULL),
attribute_shared_cache_wrapper(NULL),
acq_timestamp_cache_index(0),
device_event_channel(NULL){
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

const string& AbstractControlUnit::getCUParam() {
	return control_unit_param;
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
	
	//undocumented field
	setupConfiguration.addStringValue("mds_control_key", control_key);
	
	//check if as been setuped a file for configuration
	//LCU_ << "Check if as been setup a json file path to configura CU:" << CU_IDENTIFIER_C_STREAM;
	//loadCDataWrapperForJsonFile(setupConfiguration);
	
	//first call the setup abstract method used by the implementing CU to define action, dataset and other
	//usefull value
	ACULDBG_ << "Define Actions and Dataset";
	unitDefineActionAndDataset();
	
	//for now we need only to add custom action for expose to rpc
	//input element of the dataset
	ACULDBG_ << "Define the base action for map the input attribute of the dataset";
	AbstActionDescShrPtr
	actionDescription = addActionDescritionInstance<AbstractControlUnit>(this,
																		 &AbstractControlUnit::_setDatasetAttribute,
																		 "setDatasetAttribute",
																		 "method for set the input element for the dataset");
	
	//expose updateConfiguration Methdo to rpc
	ACULDBG_ << "Register updateConfiguration action";
	addActionDescritionInstance<AbstractControlUnit>(this,
													 &AbstractControlUnit::updateConfiguration,
													 "updateConfiguration",
													 "Update control unit configuration");
	
	ACULDBG_ << "Register initDevice action";
	addActionDescritionInstance<AbstractControlUnit>(this,
													 &AbstractControlUnit::_init,
													 ChaosSystemDomainAndActionLabel::ACTION_CU_INIT,
													 "Perform the control unit initialization");
	
	ACULDBG_ << "Register deinitDevice action";
	addActionDescritionInstance<AbstractControlUnit>(this,
													 &AbstractControlUnit::_deinit,
													 ChaosSystemDomainAndActionLabel::ACTION_CU_DEINIT,
													 "Perform the control unit deinitialization");
	ACULDBG_ << "Register startDevice action";
	addActionDescritionInstance<AbstractControlUnit>(this,
													 &AbstractControlUnit::_start,
													 ChaosSystemDomainAndActionLabel::ACTION_CU_START,
													 "Start the control unit scheduling");
	
	ACULDBG_ << "Register stopDevice action";
	addActionDescritionInstance<AbstractControlUnit>(this,
													 &AbstractControlUnit::_stop,
													 ChaosSystemDomainAndActionLabel::ACTION_CU_STOP,
													 "Stop the control unit scheduling");
	ACULDBG_ << "Register getState action";
	addActionDescritionInstance<AbstractControlUnit>(this,
													 &AbstractControlUnit::_getState,
													 ChaosSystemDomainAndActionLabel::ACTION_CU_GET_STATE,
													 "Get the state of the running control unit");
	ACULDBG_ << "Register getInfo action";
	addActionDescritionInstance<AbstractControlUnit>(this,
													 &AbstractControlUnit::_getInfo,
													 ChaosSystemDomainAndActionLabel::ACTION_CU_GET_INFO,
													 "Get the information about running control unit");
	
	ACULDBG_ << "Get dataset description";
	//grab dataset description
	DatasetDB::fillDataWrapperWithDataSetDescription(setupConfiguration);
	
#if DEBUG
	ACULDBG_ << setupConfiguration.getJSONString();
#endif
}

void AbstractControlUnit::unitDefineDriver(std::vector<DrvRequestInfo>& neededDriver) {
	
	for(ControlUnitDriverListIterator iter = control_unit_drivers.begin();
		iter != control_unit_drivers.end();
		iter++) {
		//copy driver info to the system array reference
		neededDriver.push_back(*iter);
	}
}

void AbstractControlUnit::unitDefineCustomAttribute() {
	
}

/*
 Define the control unit DataSet and Action into
 a CDataWrapper
 */
void AbstractControlUnit::_undefineActionAndDataset() throw(CException) {
	ACULDBG_ << "Remove Action Description";
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
	if(!attribute_value_shared_cache) throw CException(-1, "No Shared cache implementation found", __PRETTY_FUNCTION__);
	
	std::vector<string> attribute_names;
	utility::StartableService::initImplementation(this, static_cast<void*>(initConfiguration), "AbstractControlUnit", __PRETTY_FUNCTION__);
	
	//the init of the implementation unit goes after the infrastructure one
	ACULDBG_ << "Start internal and custom inititialization";
	try {
		ACULAPP_ << "Allcoate the user cache wrapper";
		attribute_shared_cache_wrapper = new AttributeSharedCacheWrapper(attribute_value_shared_cache);
		
		ACULAPP_ << "Populating shared attribute cache for input attribute";
		DatasetDB::getDatasetAttributesName(DataType::Input, attribute_names);
		initAttributeOnSharedAttributeCache(AttributeValueSharedCache::SVD_INPUT, attribute_names);
		
		ACULAPP_ << "Populating shared attribute cache for output attribute";
		attribute_names.clear();
		DatasetDB::getDatasetAttributesName(DataType::Output, attribute_names);
		initAttributeOnSharedAttributeCache(AttributeValueSharedCache::SVD_OUTPUT, attribute_names);
		
		ACULAPP_ << "Complete shared attribute cache for output attribute";
		completeOutputAttribute();
		
		ACULAPP_ << "Complete shared attribute cache for input attribute";
		completeInputAttribute();
		
		ACULAPP_ << "Populating shared attribute cache for system attribute";
		initSystemAttributeOnSharedAttributeCache();

		
		//define the implementations custom variable
		unitDefineCustomAttribute();
		
		//create fast vector access for cached value
		fillCachedValueVector(attribute_value_shared_cache->getSharedDomain(AttributeValueSharedCache::SVD_OUTPUT), cache_output_attribute_vector);
		
		fillCachedValueVector(attribute_value_shared_cache->getSharedDomain(AttributeValueSharedCache::SVD_INPUT), cache_input_attribute_vector);
		
		fillCachedValueVector(attribute_value_shared_cache->getSharedDomain(AttributeValueSharedCache::SVD_SYSTEM), cache_system_attribute_vector);
		
		fillCachedValueVector(attribute_value_shared_cache->getSharedDomain(AttributeValueSharedCache::SVD_CUSTOM), cache_custom_attribute_vector);
		
		//initialize implementations
		unitInit();
		
		//call update param function
		updateConfiguration(initConfiguration, detachParam);
	}catch(CException& ex) {
		//inthis case i need to deinit the state of the abstract control unit
		try{
			bool detach;
			_deinit(NULL, detach);
		} catch(CException& sub_ex) {}
		throw ex;
	}
	
	return NULL;
}

/*
 Starto the  Control Unit scheduling for device
 */
CDataWrapper* AbstractControlUnit::_start(CDataWrapper *startParam, bool& detachParam) throw(CException) {
	//call start method of the startable interface
	utility::StartableService::startImplementation(this, "AbstractControlUnit", __PRETTY_FUNCTION__);
	ACULDBG_ << "Start sublass for deviceID:" << DatasetDB::getDeviceID();
	try {
		unitStart();
	}catch(CException& ex) {
		//inthis case i need to stop the abstract control unit
		try{
			bool detach;
			_stop(NULL, detach);
		} catch(CException& sub_ex) {}
		
		throw ex;
	}
	return NULL;
}

/*
 Stop the Custom Control Unit scheduling for device
 */
CDataWrapper* AbstractControlUnit::_stop(CDataWrapper *stopParam, bool& detachParam) throw(CException) {
	//first we start the deinitializaiton of the implementation unit
	try {
		ACULDBG_ << "Stop sublass for deviceID:" << DatasetDB::getDeviceID();
		unitStop();
	} catch (CException& ex) {
		ACULDBG_ << "Exception on unit deinit:" << ex.what();
	}

	//call start method of the startable interface
	utility::StartableService::stopImplementation(this, "AbstractControlUnit", __PRETTY_FUNCTION__);
	//first we need to stop the implementation unit
	return NULL;
}

/*
 deinit all datastorage
 */
CDataWrapper* AbstractControlUnit::_deinit(CDataWrapper *deinitParam, bool& detachParam) throw(CException) {
	
 	//first we start the deinitializaiton of the implementation unit
	try {
		ACULDBG_ << "Deinit custom deinitialization for device:" << DatasetDB::getDeviceID();
		unitDeinit();
		
		//saftely deinititalize the abstract control unit
		try {
			utility::StartableService::deinitImplementation(this, "AbstractControlUnit", __PRETTY_FUNCTION__);
		} catch (CException& ex) {
			ACULDBG_ << "Exception on abstract control l unit deinit:" << ex.what();
		}

		//clear all cache sub_structure
		cache_output_attribute_vector.clear();
		cache_input_attribute_vector.clear();
		cache_custom_attribute_vector.clear();
		cache_system_attribute_vector.clear();
		
		ACULAPP_ << "Dellcocate the user cache wrapper";
		if(attribute_shared_cache_wrapper) {
			delete(attribute_shared_cache_wrapper);
			attribute_shared_cache_wrapper = NULL;
		}			
	} catch (CException& ex) {
		ACULDBG_ << "Exception on unit deinit:" << ex.what();
	}
	return NULL;
}

/*
 Receive the event for set the dataset input element
 */
CDataWrapper* AbstractControlUnit::_setDatasetAttribute(CDataWrapper *datasetAttributeValues,  bool& detachParam) throw (CException) {
	CDataWrapper *executionResult = NULL;
	try {
		if(!datasetAttributeValues) {
			throw CException(-1, "No Input parameter", __PRETTY_FUNCTION__);
		}
		
#if DEBUG
		ACULDBG_ << datasetAttributeValues->getJSONString();
#endif
		
		if(!datasetAttributeValues->hasKey(DatasetDefinitionkey::DEVICE_ID)) {
			throw CException(-2, "No Device Defined in param", __PRETTY_FUNCTION__);
		}
		//retrive the deviceid
		string deviceID = datasetAttributeValues->getStringValue(DatasetDefinitionkey::DEVICE_ID);
		if(utility::StartableService::getServiceState() == CUStateKey::DEINIT) {
			throw CException(-3, "The Control Unit is in deinit state", __PRETTY_FUNCTION__);
		}
		//send dataset attribute change pack to control unit implementation
		executionResult = setDatasetAttribute(datasetAttributeValues, detachParam);
		
	} catch (CException& ex) {
		//at this time notify the wel gone setting of comand
		throw ex;
	}
	
	return executionResult;
}

// Startable Service method
void AbstractControlUnit::init(void *initData) throw(CException) {
	CDataWrapper *initConfiguration = static_cast<CDataWrapper*>(initData);
	if(!initConfiguration ||
	   !initConfiguration->hasKey(DatasetDefinitionkey::DEVICE_ID)) {
		throw CException(-1, "No Device Init information in param", __PRETTY_FUNCTION__);
	}
	
	std::string deviceID = initConfiguration->getStringValue(DatasetDefinitionkey::DEVICE_ID);
	if(deviceID.compare(DatasetDB::getDeviceID())) {
		ACULERR_ << "device:" << deviceID << "not known by this Work Unit";
		throw CException(-2, "Device not known by this control unit", __PRETTY_FUNCTION__);
	}
	ACULAPP_ << "Initializating Phase for device:" << deviceID;
	//at this point and before the unit implementation init i need to get
	//the infromation about the needed drivers
	std::vector<DrvRequestInfo> unitNeededDrivers;
	
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
	//remove key data storage
	if(keyDataStorage) {
		ACULDBG_ << "Delete data storage driver for device:" << DatasetDB::getDeviceID();
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
	

}

void AbstractControlUnit::fillCachedValueVector(AttributesSetting& attribute_cache,
												  std::vector<ValueSetting*>& cached_value) {
	for(int idx = 0;
		idx < attribute_cache.getNumberOfAttributes();
		idx++) {
		cached_value.push_back(attribute_cache.getValueSettingForIndex(idx));
	}
}

void AbstractControlUnit::initAttributeOnSharedAttributeCache(AttributeValueSharedCache::SharedVariableDomain domain,
															  std::vector<string>& attribute_names) {
	//add input attribute to shared setting
	RangeValueInfo attributeInfo;
	
	AttributesSetting& attribute_setting = attribute_value_shared_cache->getSharedDomain(domain);
	
	for(int idx = 0;
		idx < attribute_names.size();
		idx++) {
		
		attributeInfo.reset();
		
		// retrive the attribute description from the device database
		DatasetDB::getAttributeRangeValueInfo(attribute_names[idx], attributeInfo);
		
		// add the attribute to the shared setting object
		attribute_setting.addAttribute(attribute_names[idx], attributeInfo.maxSize, attributeInfo.valueType);
		
		if(!attributeInfo.defaultValue.size()) continue;
		
		//setting value using index (the index into the sharedAttributeSetting are sequencial to the inserted order)
		switch (attributeInfo.valueType) {
			case DataType::TYPE_BOOLEAN : {
				bool val = boost::lexical_cast<bool>(attributeInfo.defaultValue);
				attribute_setting.setValueForAttribute(idx, &val, sizeof(bool));
				break;}
			case DataType::TYPE_DOUBLE : {
				double val = boost::lexical_cast<double>(attributeInfo.defaultValue);
				attribute_setting.setValueForAttribute(idx, &val, sizeof(double));
				break;}
			case DataType::TYPE_INT32 : {
				int32_t val = boost::lexical_cast<int32_t>(attributeInfo.defaultValue);
				attribute_setting.setValueForAttribute(idx, &val, sizeof(int32_t));
				break;}
			case DataType::TYPE_INT64 : {
				int64_t val = boost::lexical_cast<int64_t>(attributeInfo.defaultValue);
				attribute_setting.setValueForAttribute(idx, &val, sizeof(int64_t));
				break;}
			case DataType::TYPE_STRING : {
				const char * val = attributeInfo.defaultValue.c_str();
				attribute_setting.setValueForAttribute(idx, val, (uint32_t)attributeInfo.defaultValue.size());
				break;}
			case DataType::TYPE_BYTEARRAY: {
				ACULDBG_ << "Binary default setting has not been yet managed";
				break;
			}
			default:
				break;
		}
	}
}

void AbstractControlUnit::completeOutputAttribute() {
	ACULDBG_ << "Complete the shared cache output attribute";
	AttributesSetting& domain_attribute_setting = attribute_value_shared_cache->getSharedDomain(AttributeValueSharedCache::SVD_OUTPUT);
	
	std::string device_id_str = DatasetDB::getDeviceID();
	domain_attribute_setting.addAttribute(DataPackKey::CS_CSV_CU_ID, (uint32_t)device_id_str.size(), DataType::TYPE_STRING);
	domain_attribute_setting.setValueForAttribute((domain_attribute_setting.getNumberOfAttributes()-1), device_id_str.c_str(), (uint32_t)device_id_str.size());
	
	domain_attribute_setting.addAttribute(DataPackKey::CS_CSV_TIME_STAMP, sizeof(uint64_t), DataType::TYPE_INT64);
	
	//get the timestamp index
	timestamp_acq_cache_index = domain_attribute_setting.getNumberOfAttributes() - 1;
}

void AbstractControlUnit::completeInputAttribute() {
	
}

void AbstractControlUnit::initSystemAttributeOnSharedAttributeCache() {
	AttributesSetting& domain_attribute_setting = attribute_value_shared_cache->getSharedDomain(AttributeValueSharedCache::SVD_SYSTEM);
	//add heart beat attribute
	ACULDBG_ << "Adding syste attribute on shared cache";
	domain_attribute_setting.addAttribute(DataPackSystemKey::DP_SYS_HEARTBEAT, 0, DataType::TYPE_INT64);
	
	//add unit type
	domain_attribute_setting.addAttribute(DataPackSystemKey::DP_SYS_UNIT_TYPE, (uint32_t)control_unit_type.size(), DataType::TYPE_STRING);
	domain_attribute_setting.setValueForAttribute(domain_attribute_setting.getNumberOfAttributes()-1, control_unit_type.c_str(),  (uint32_t)control_unit_type.size());
	
	//add error attribute
	domain_attribute_setting.addAttribute(DataPackSystemKey::DP_SYS_LAST_ERROR, 0, DataType::TYPE_INT32);
	
	//add error message attribute
	domain_attribute_setting.addAttribute(DataPackSystemKey::DP_SYS_LAST_ERROR_MESSAGE, 255, DataType::TYPE_STRING);
}

/*
 Get the current control unit state
 */
CDataWrapper* AbstractControlUnit::_getState(CDataWrapper* getStatedParam, bool& detachParam) throw(CException) {
	if(!getStatedParam->hasKey(DatasetDefinitionkey::DEVICE_ID)){
		throw CException(-1, "Get State Pack without DeviceID", __PRETTY_FUNCTION__);
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

//! this andler is called befor the input attribute will be updated
void AbstractControlUnit::unitInputAttributePreChangeHandler() throw(CException) {
	
}

//! attribute change handler
/*!
 the handle is fired after the input attribute cache as been update triggere
 by the rpc request for attribute change.
 */
void AbstractControlUnit::unitInputAttributeChangedHandler() throw(CException) {
	
}

#define CHECK_FOR_RANGE_VALUE(t, v, attr_name)\
t max = attributeInfo.maxRange.size()?boost::lexical_cast<t>(attributeInfo.maxRange):std::numeric_limits<t>::max();\
t min = attributeInfo.maxRange.size()?boost::lexical_cast<t>(attributeInfo.minRange):std::numeric_limits<t>::min();\
if(v < min || v > max) throw CException(-1,  boost::str(boost::format("Invalid value (%1%) [max:%2% Min:%3%] for attribute %4%") % v % attr_name % attributeInfo.minRange % attributeInfo.maxRange).c_str(), __PRETTY_FUNCTION__);\

#define CHECK_FOR_STRING_RANGE_VALUE(v, attr_name)\
if(attributeInfo.minRange.size() && v < attributeInfo.minRange ) throw CException(-1, boost::str(boost::format("Invalid value (%1%) [max:%2% Min:%3%] for attribute %4%") % v % attr_name % attributeInfo.minRange % attributeInfo.maxRange).c_str(), __PRETTY_FUNCTION__);\
if(attributeInfo.maxRange.size() && v > attributeInfo.maxRange ) throw CException(-1, boost::str(boost::format("Invalid value (%1%) [max:%2% Min:%3%] for attribute %4%") % v % attr_name %attributeInfo.minRange % attributeInfo.maxRange).c_str(), __PRETTY_FUNCTION__);\

CDataWrapper* AbstractControlUnit::setDatasetAttribute(CDataWrapper *dataset_attribute_values, bool& detachParam) throw (CException) {
	CHAOS_ASSERT(dataset_attribute_values)
	boost::shared_ptr<SharedCacheLockDomain> w_lock = attribute_value_shared_cache->getLockOnDomain(AttributeValueSharedCache::SVD_INPUT, true);
	w_lock->lock();

	std::vector<std::string> in_attribute_name;
	RangeValueInfo attributeInfo;
	try {
		//call pre handler
		unitInputAttributePreChangeHandler();
		
		//get all input attribute name
		getDatasetAttributesName(DataType::Input , in_attribute_name);
		
		if(dataset_attribute_values->hasKey(DatasetDefinitionkey::DEVICE_ID)) {
			std::string _messageDeviceID = dataset_attribute_values->getStringValue(DatasetDefinitionkey::DEVICE_ID);
			//compare the message device id and the local
			for (std::vector<std::string>::iterator iter = in_attribute_name.begin();
				 iter != in_attribute_name.end();
				 iter++) {
				//execute attribute handler
				const char * cAttrName = iter->c_str();
				
				//check if the attribute name is present
				if(dataset_attribute_values->hasKey(cAttrName)) {
					
					ValueSetting * attribute_cache_value = attribute_value_shared_cache->getVariableValue(SharedCacheInterface::SVD_INPUT, iter->c_str());
					
					//get attribute info
					getAttributeRangeValueInfo(*iter, attributeInfo);
					//call handler
					switch (attribute_cache_value->type) {
						case DataType::TYPE_BOOLEAN: {
							bool bv = dataset_attribute_values->getBoolValue(cAttrName);
							attribute_cache_value->setValue(&bv, sizeof(bool));
							break;
						}
						case DataType::TYPE_INT32: {
							int32_t i32v = dataset_attribute_values->getInt32Value(cAttrName);
							CHECK_FOR_RANGE_VALUE(int32_t, i32v, cAttrName)
							attribute_cache_value->setValue(&i32v, sizeof(int32_t));
							break;
						}
						case DataType::TYPE_INT64: {
							int64_t i64v = dataset_attribute_values->getInt64Value(cAttrName);
							CHECK_FOR_RANGE_VALUE(int64_t, i64v, cAttrName)
							attribute_cache_value->setValue(&i64v, sizeof(int64_t));
							break;
						}
						case DataType::TYPE_DOUBLE: {
							double dv = dataset_attribute_values->getDoubleValue(cAttrName);
							CHECK_FOR_RANGE_VALUE(double, dv, cAttrName)
							attribute_cache_value->setValue(&dv, sizeof(double));
							break;
						}
						case DataType::TYPE_STRING: {
							std::string str = dataset_attribute_values->getStringValue(cAttrName);
							CHECK_FOR_STRING_RANGE_VALUE(str, cAttrName)
							attribute_cache_value->setValue(str.c_str(), (uint32_t)str.size());
							break;
						}
						case DataType::TYPE_BYTEARRAY: {
							int bin_size = 0;
							const char *binv = dataset_attribute_values->getBinaryValue(cAttrName, bin_size);
							attribute_cache_value->setValue(binv, bin_size);
							break;
						}
					}
				}
			}
			
			//push the input attribute dataset
			pushInputDataset();
		}
		
		//inform the subclass for the change
		unitInputAttributeChangedHandler();
	}catch(CException& ex) {
		
		//inform the subclass for the change
		unitInputAttributeChangedHandler();
		
		throw ex;
	}
	
	//at this time notify the wel gone setting of comand
	//if(deviceEventChannel) deviceEventChannel->notifyForAttributeSetting(DatasetDB::getDeviceID(), 0);
	return NULL;
}

/*
 Update the configuration for all descendand tree in the Control Uniti class struccture
 */
CDataWrapper*  AbstractControlUnit::updateConfiguration(CDataWrapper* updatePack, bool& detachParam) throw (CException) {
	//load all keyDataStorageMap for the registered devices
	if(!updatePack || !updatePack->hasKey(DatasetDefinitionkey::DEVICE_ID)) {
		throw CException(-1, "Update pack without DeviceID", __PRETTY_FUNCTION__);
	}
	
	string deviceID = updatePack->getStringValue(DatasetDefinitionkey::DEVICE_ID);
	
	if(deviceID.compare(DatasetDB::getDeviceID())) {
		ACULAPP_ << "device:" << DatasetDB::getDeviceID() << "not known by this ContorlUnit";
		throw CException(-2, "Device not known by this control unit", __PRETTY_FUNCTION__);
	}
	
	//check to see if the device can ben initialized
	if(utility::StartableService::getServiceState() == INIT_STATE) {
		ACULAPP_ << "device:" << DatasetDB::getDeviceID() << " not initialized";
		throw CException(-3, "Device Not Initilized", __PRETTY_FUNCTION__);
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
DriverAccessor *AbstractControlUnit::getAccessoInstanceByIndex(int idx) {
	if( idx >= accessorInstances.size() ) return NULL;
	return accessorInstances[idx];
}


void AbstractControlUnit::pushOutputDataset() {
	AttributesSetting& output_attribute_cache = attribute_value_shared_cache->getSharedDomain(AttributeValueSharedCache::SVD_OUTPUT);
	boost::shared_ptr<SharedCacheLockDomain> r_lock = attribute_value_shared_cache->getLockOnDomain(AttributeValueSharedCache::SVD_OUTPUT, false);
	r_lock->lock();
	
	//check if something as changed
	if(!output_attribute_cache.hasChanged()) return;
	
	CDataWrapper *output_attribute_dataset = keyDataStorage->getNewOutputAttributeDataWrapper();
	if(!output_attribute_dataset) return;
	
	//the device id is preinsert by keyDataStorage
	
	//write acq ts for second
	ValueSetting * value_set = cache_output_attribute_vector[timestamp_acq_cache_index];
	output_attribute_dataset->addInt64Value(value_set->name.c_str(), *value_set->getValuePtr<int64_t>());
	
	//add all other output channel
	for(int idx = 0;
		idx < cache_output_attribute_vector.size() - 2; //the device id and timestamp in added out of this list
		idx++) {
		//
		ValueSetting * value_set = cache_output_attribute_vector[idx];
		switch(value_set->type) {
			case DataType::TYPE_BOOLEAN:
				output_attribute_dataset->addBoolValue(value_set->name.c_str(), *value_set->getValuePtr<bool>());
				break;
			case DataType::TYPE_INT32:
				output_attribute_dataset->addInt32Value(value_set->name.c_str(), *value_set->getValuePtr<int32_t>());
				break;
			case DataType::TYPE_INT64:
				output_attribute_dataset->addInt64Value(value_set->name.c_str(), *value_set->getValuePtr<int64_t>());
				break;
			case DataType::TYPE_DOUBLE:
				output_attribute_dataset->addDoubleValue(value_set->name.c_str(), *value_set->getValuePtr<double>());
				break;
			case DataType::TYPE_STRING:
				output_attribute_dataset->addStringValue(value_set->name.c_str(), value_set->getValuePtr<const char>());
				break;
			case DataType::TYPE_BYTEARRAY:
				output_attribute_dataset->addBinaryValue(value_set->name.c_str(), value_set->getValuePtr<char>(), value_set->size);
				break;
		}
	}
	
	//now we nede to push the outputdataset
	keyDataStorage->pushDataSet(data_manager::KeyDataStorageDomainOutput, output_attribute_dataset);
	
	//reset chagned attribute into output dataset
	output_attribute_cache.resetChangedIndex();
}

//push system dataset
void AbstractControlUnit::pushInputDataset() {
	AttributesSetting& input_attribute_cache = attribute_value_shared_cache->getSharedDomain(AttributeValueSharedCache::SVD_INPUT);
	if(!input_attribute_cache.hasChanged()) return;
	//get the cdatawrapper for the pack
	CDataWrapper *input_attribute_dataset = keyDataStorage->getNewOutputAttributeDataWrapper();
	if(input_attribute_dataset) {
		//fill the dataset
		fillCDatawrapperWithCachedValue(cache_input_attribute_vector, *input_attribute_dataset);
		
		//push out the system dataset
		keyDataStorage->pushDataSet(data_manager::KeyDataStorageDomainInput, input_attribute_dataset);
	}
}

//push system dataset
void AbstractControlUnit::pushCustomDataset() {
	AttributesSetting& custom_attribute_cache = attribute_value_shared_cache->getSharedDomain(AttributeValueSharedCache::SVD_CUSTOM);
	if(!custom_attribute_cache.hasChanged()) return;
	//get the cdatawrapper for the pack
	CDataWrapper *custom_attribute_dataset = keyDataStorage->getNewOutputAttributeDataWrapper();
	if(custom_attribute_dataset) {
		//fill the dataset
		fillCDatawrapperWithCachedValue(cache_custom_attribute_vector, *custom_attribute_dataset);
		
		//push out the system dataset
		keyDataStorage->pushDataSet(data_manager::KeyDataStorageDomainCustom, custom_attribute_dataset);
	}
}

void AbstractControlUnit::pushSystemDataset() {
	AttributesSetting& systemm_attribute_cache = attribute_value_shared_cache->getSharedDomain(AttributeValueSharedCache::SVD_SYSTEM);
	if(!systemm_attribute_cache.hasChanged()) return;
	//get the cdatawrapper for the pack
	CDataWrapper *system_attribute_dataset = keyDataStorage->getNewOutputAttributeDataWrapper();
	if(system_attribute_dataset) {
		//fill the dataset
		fillCDatawrapperWithCachedValue(cache_system_attribute_vector, *system_attribute_dataset);
		
		//push out the system dataset
		keyDataStorage->pushDataSet(data_manager::KeyDataStorageDomainSystem, system_attribute_dataset);
	}
	//reset changed index
	systemm_attribute_cache.resetChangedIndex();
}

void AbstractControlUnit::fillCDatawrapperWithCachedValue(std::vector<ValueSetting*>& cached_attributes, CDataWrapper& dataset) {
	for(std::vector<ValueSetting*>::iterator it = cached_attributes.begin();
		it != cached_attributes.end();
		it++) {
		
		switch((*it)->type) {
			case DataType::TYPE_BOOLEAN:
				dataset.addBoolValue((*it)->name.c_str(), *(*it)->getValuePtr<bool>());
				break;
			case DataType::TYPE_INT32:
				dataset.addInt32Value((*it)->name.c_str(), *(*it)->getValuePtr<int32_t>());
				break;
			case DataType::TYPE_INT64:
				dataset.addInt64Value((*it)->name.c_str(), *(*it)->getValuePtr<int64_t>());
				break;
			case DataType::TYPE_DOUBLE:
				dataset.addDoubleValue((*it)->name.c_str(), *(*it)->getValuePtr<double>());
				break;
			case DataType::TYPE_STRING:
				dataset.addStringValue((*it)->name.c_str(), (*it)->getValuePtr<const char>());
				break;
			case DataType::TYPE_BYTEARRAY:
				dataset.addBinaryValue((*it)->name.c_str(), (*it)->getValuePtr<char>(), (*it)->size);
				break;
		}
	}
}

