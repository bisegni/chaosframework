/*
 *	ControlUnit.cpp
 *	!CHAOS
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
#include <chaos/common/healt_system/HealtManager.h>
#include <chaos/common/event/channel/InstrumentEventChannel.h>

#include <chaos/cu_toolkit/DataManager/DataManager.h>
#include <chaos/cu_toolkit/driver_manager/DriverManager.h>
#include <chaos/cu_toolkit/CommandManager/CommandManager.h>
#include <chaos/cu_toolkit/ControlManager/AbstractControlUnit.h>

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
using namespace chaos::common::utility;
using namespace chaos::common::data::cache;
using namespace chaos::common::healt_system;

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
timestamp_acq_cached_value(NULL),
key_data_storage(NULL){
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
timestamp_acq_cached_value(NULL),
key_data_storage(NULL){
    //copy array
    for (int idx = 0; idx < _control_unit_drivers.size(); idx++){
        control_unit_drivers.push_back(_control_unit_drivers[idx]);
    }
    
    ACULAPP_ << "Initializating Driver Accessors";
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
    
}

/*!
 Destructor a new CU with an identifier
 */
AbstractControlUnit::~AbstractControlUnit() {
    //clear the accessor of the driver
    for (int idx = 0;
         idx != accessorInstances.size();
         idx++) {
        driver_manager::DriverManager::getInstance()->releaseAccessor(accessorInstances[idx]);
    }
    //clear the vector
    accessorInstances.clear();
    
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
void AbstractControlUnit::setKeyDataStorage(KeyDataStorage* _key_data_storage) {
    key_data_storage = _key_data_storage;
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
    setupConfiguration.addStringValue(NodeDefinitionKey::NODE_RPC_DOMAIN, control_unit_instance);
    
    //undocumented field
    setupConfiguration.addStringValue("mds_control_key", control_key);
    
    //add the control unit type with semantonc type::subtype
    setupConfiguration.addStringValue(NodeDefinitionKey::NODE_TYPE, NodeType::NODE_TYPE_CONTROL_UNIT);
    
    //check if as been setuped a file for configuration
    //LCU_ << "Check if as been setup a json file path to configura CU:" << CU_IDENTIFIER_C_STREAM;
    //loadCDataWrapperForJsonFile(setupConfiguration);
    
    //first call the setup abstract method used by the implementing CU to define action, dataset and other
    //usefull value
    unitDefineActionAndDataset();
    
    //for now we need only to add custom action for expose to rpc
    //input element of the dataset
    AbstActionDescShrPtr
    actionDescription = addActionDescritionInstance<AbstractControlUnit>(this,
                                                                         &AbstractControlUnit::_setDatasetAttribute,
                                                                         "setDatasetAttribute",
                                                                         "method for set the input element for the dataset");
    
    //expose updateConfiguration Methdo to rpc
    addActionDescritionInstance<AbstractControlUnit>(this,
                                                     &AbstractControlUnit::updateConfiguration,
                                                     "updateConfiguration",
                                                     "Update control unit configuration");
    
    addActionDescritionInstance<AbstractControlUnit>(this,
                                                     &AbstractControlUnit::_init,
                                                     NodeDomainAndActionRPC::ACTION_NODE_INIT,
                                                     "Perform the control unit initialization");
    
    addActionDescritionInstance<AbstractControlUnit>(this,
                                                     &AbstractControlUnit::_deinit,
                                                     NodeDomainAndActionRPC::ACTION_NODE_DEINIT
                                                     ,
                                                     "Perform the control unit deinitialization");
    addActionDescritionInstance<AbstractControlUnit>(this,
                                                     &AbstractControlUnit::_start,
                                                     NodeDomainAndActionRPC::ACTION_NODE_START,
                                                     "Start the control unit scheduling");
    
    addActionDescritionInstance<AbstractControlUnit>(this,
                                                     &AbstractControlUnit::_stop,
                                                     NodeDomainAndActionRPC::ACTION_NODE_STOP,
                                                     "Stop the control unit scheduling");
    addActionDescritionInstance<AbstractControlUnit>(this,
                                                     &AbstractControlUnit::_unitRestoreToSnapshot,
                                                     NodeDomainAndActionRPC::ACTION_NODE_RESTORE,
                                                     "Restore contorl unit to a snapshot tag");
    addActionDescritionInstance<AbstractControlUnit>(this,
                                                     &AbstractControlUnit::_getState,
                                                     NodeDomainAndActionRPC::ACTION_NODE_GET_STATE,
                                                     "Get the state of the running control unit");
    addActionDescritionInstance<AbstractControlUnit>(this,
                                                     &AbstractControlUnit::_getInfo,
                                                     NodeDomainAndActionRPC::ACTION_CU_GET_INFO,
                                                     "Get the information about running control unit");
    
    //grab dataset description
    DatasetDB::fillDataWrapperWithDataSetDescription(setupConfiguration);
    
    //get action description
    getActionDescrionsInDataWrapper(setupConfiguration);
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
CDataWrapper* AbstractControlUnit::_init(CDataWrapper *init_configuration,
                                         bool& detachParam) throw(CException) {
    if(!attribute_value_shared_cache) throw CException(-1, "No Shared cache implementation found for:"+DatasetDB::getDeviceID(), __PRETTY_FUNCTION__);
    
    std::vector<string> attribute_names;
    try {
        HealtManager::getInstance()->addNodeMetricValue(control_unit_id,
                                                        NodeHealtDefinitionKey::NODE_HEALT_STATUS,
                                                        NodeHealtDefinitionValue::NODE_HEALT_STATUS_INITING,
                                                        true);
        
        StartableService::initImplementation(this, static_cast<void*>(init_configuration), "AbstractControlUnit", __PRETTY_FUNCTION__);
        
        //the init of the implementation unit goes after the infrastructure one
        ACULDBG_ << "Start internal and custom inititialization:"+DatasetDB::getDeviceID();
        
        
        ACULAPP_ << "Allocate the user cache wrapper for:"+DatasetDB::getDeviceID();
        attribute_shared_cache_wrapper = new AttributeSharedCacheWrapper(attribute_value_shared_cache);
        
        ACULAPP_ << "Populating shared attribute cache for input attribute";
        DatasetDB::getDatasetAttributesName(DataType::Input, attribute_names);
        initAttributeOnSharedAttributeCache(DOMAIN_INPUT, attribute_names);
        
        ACULAPP_ << "Populating shared attribute cache for output attribute";
        attribute_names.clear();
        DatasetDB::getDatasetAttributesName(DataType::Output, attribute_names);
        initAttributeOnSharedAttributeCache(DOMAIN_OUTPUT, attribute_names);
        
        ACULAPP_ << "Complete shared attribute cache for output attribute";
        completeOutputAttribute();
        
        ACULAPP_ << "Complete shared attribute cache for input attribute";
        completeInputAttribute();
        
        ACULAPP_ << "Populating shared attribute cache for system attribute";
        initSystemAttributeOnSharedAttributeCache();
        
        
        //define the implementations custom variable
        unitDefineCustomAttribute();
        
        //create fast vector access for cached value
        fillCachedValueVector(attribute_value_shared_cache->getSharedDomain(DOMAIN_OUTPUT),
                              cache_output_attribute_vector);
        
        fillCachedValueVector(attribute_value_shared_cache->getSharedDomain(DOMAIN_INPUT),
                              cache_input_attribute_vector);
        
        fillCachedValueVector(attribute_value_shared_cache->getSharedDomain(DOMAIN_SYSTEM),
                              cache_system_attribute_vector);
        
        fillCachedValueVector(attribute_value_shared_cache->getSharedDomain(DOMAIN_CUSTOM),
                              cache_custom_attribute_vector);
        
        //initialize implementations
        unitInit();
        
        //call update param function
        updateConfiguration(init_configuration, detachParam);
        
        //set healt to init
        HealtManager::getInstance()->addNodeMetricValue(control_unit_id,
                                                        NodeHealtDefinitionKey::NODE_HEALT_STATUS,
                                                        NodeHealtDefinitionValue::NODE_HEALT_STATUS_INIT,
                                                        true);
    }catch(CException& ex) {
        //inthis case i need to deinit the state of the abstract control unit
        try{
            ACULAPP_ <<"Exception initializing  \""<< DatasetDB::getDeviceID()<< "\":"<<ex.what()<<", deinit...";
            bool detach;
            _deinit(NULL, detach);
        } catch(CException& sub_ex) {}
        HealtManager::getInstance()->addNodeMetricValue(control_unit_id,
                                                        NodeHealtDefinitionKey::NODE_HEALT_STATUS,
                                                        NodeHealtDefinitionValue::NODE_HEALT_STATUS_DEINIT,
                                                        true);
        throw ex;
    }
    return NULL;
}

/*
 Starto the  Control Unit scheduling for device
 */
CDataWrapper* AbstractControlUnit::_start(CDataWrapper *startParam,
                                          bool& detachParam) throw(CException) {
    //call start method of the startable interface
    ACULDBG_ << "Start sublass for deviceID:" << DatasetDB::getDeviceID();
    StartableService::startImplementation(this, "AbstractControlUnit", __PRETTY_FUNCTION__);
    
    try {
        HealtManager::getInstance()->addNodeMetricValue(control_unit_id,
                                                        NodeHealtDefinitionKey::NODE_HEALT_STATUS,
                                                        NodeHealtDefinitionValue::NODE_HEALT_STATUS_STARTING,
                                                        true);
        unitStart();
        
        //set healt to start
        HealtManager::getInstance()->addNodeMetricValue(control_unit_id,
                                                        NodeHealtDefinitionKey::NODE_HEALT_STATUS,
                                                        NodeHealtDefinitionValue::NODE_HEALT_STATUS_START,
                                                        true);
    }catch(CException& ex) {
        //inthis case i need to stop the abstract control unit
        try{
            bool detach;
            ACULAPP_ <<"Exception starting  \""<< DatasetDB::getDeviceID()<< "\":"<<ex.what()<<", stopping...";
            _stop(NULL, detach);
        } catch(CException& sub_ex) {}
        //set healt to start
        HealtManager::getInstance()->addNodeMetricValue(control_unit_id,
                                                        NodeHealtDefinitionKey::NODE_HEALT_STATUS,
                                                        NodeHealtDefinitionValue::NODE_HEALT_STATUS_STOP,
                                                        true);
        throw ex;
    }
    return NULL;
}

/*
 Stop the Custom Control Unit scheduling for device
 */
CDataWrapper* AbstractControlUnit::_stop(CDataWrapper *stopParam,
                                         bool& detachParam) throw(CException) {
    //first we start the deinitializaiton of the implementation unit
    try {
        //set healt to start
        HealtManager::getInstance()->addNodeMetricValue(control_unit_id,
                                                        NodeHealtDefinitionKey::NODE_HEALT_STATUS,
                                                        NodeHealtDefinitionValue::NODE_HEALT_STATUS_STOPING,
                                                        true);
        ACULDBG_ << "Stop sublass for deviceID:" << DatasetDB::getDeviceID();
        unitStop();
        HealtManager::getInstance()->addNodeMetricValue(control_unit_id,
                                                        NodeHealtDefinitionKey::NODE_HEALT_STATUS,
                                                        NodeHealtDefinitionValue::NODE_HEALT_STATUS_STOP,
                                                        true);
    } catch (CException& ex) {
        ACULAPP_ <<"Exception stopping  \""<< DatasetDB::getDeviceID()<< "\""<<ex.what()<<", stopping...";
        ACULDBG_ << "Exception on unit deinit:" << ex.what();
    }
    
    //call start method of the startable interface
    StartableService::stopImplementation(this, "AbstractControlUnit", __PRETTY_FUNCTION__);
    
    //set healt to stop
    HealtManager::getInstance()->addNodeMetricValue(control_unit_id,
                                                    NodeHealtDefinitionKey::NODE_HEALT_STATUS,
                                                    NodeHealtDefinitionValue::NODE_HEALT_STATUS_STOP);
    
    return NULL;
}

//! fill cache with found dataset at the restore point
void AbstractControlUnit::fillRestoreCacheWithDatasetFromTag(data_manager::KeyDataStorageDomain domain,
                                                             CDataWrapper& dataset,
                                                             AbstractSharedDomainCache& restore_cache) {
    // the list of the key
    std::vector<std::string> dataset_key;
    
    // get all key name
    dataset.getAllKey(dataset_key);
    
    uint32_t value_size = 0;
    const char * raw_value_ptr = NULL;
    AttributeValue *cached_attribute_value = NULL;
    for(std::vector<std::string>::iterator it = dataset_key.begin();
        it != dataset_key.end();
        it++) {
        //! fetch value size
        value_size = dataset.getValueSize(*it);
        
        //! fetch raw data ptr address
        raw_value_ptr = dataset.getRawValuePtr(*it);
        if(value_size &&
           raw_value_ptr) {
            //add attribute for found key and value
            restore_cache.addAttribute((SharedCacheDomain)domain,
                                       *it,
                                       value_size,
                                       chaos::DataType::TYPE_BYTEARRAY);
            
            //get newly createdattribute from cache
            cached_attribute_value = restore_cache.getAttributeValue((SharedCacheDomain)domain,
                                                                     *it);
            if(!cached_attribute_value) {
                ACULERR_ << "Error retriving attribute value from cache for:" << *it;
                continue;
            }
            
            //copy the found valu ein the cache
            std::memcpy(cached_attribute_value->value_buffer,
                        (const void *)raw_value_ptr,
                        value_size);
        }
        
        value_size = 0;
        raw_value_ptr = NULL;
    }
}

/*
 deinit all datastorage
 */
CDataWrapper* AbstractControlUnit::_deinit(CDataWrapper *deinitParam,
                                           bool& detachParam) throw(CException) {
    
    //first we start the deinitializaiton of the implementation unit
    try {
        HealtManager::getInstance()->addNodeMetricValue(control_unit_id,
                                                        NodeHealtDefinitionKey::NODE_HEALT_STATUS,
                                                        NodeHealtDefinitionValue::NODE_HEALT_STATUS_DEINITING,
                                                        true);
        
        ACULDBG_ << "Deinit custom deinitialization for device:" << DatasetDB::getDeviceID();
        unitDeinit();
        
        //saftely deinititalize the abstract control unit
        try {
            StartableService::deinitImplementation(this, "AbstractControlUnit", __PRETTY_FUNCTION__);
        } catch (CException& ex) {
            ACULAPP_ <<"Exception de-initializing  \""<< DatasetDB::getDeviceID()<< "\":"<<ex.what();
            
        }
        
        //clear all cache sub_structure
        cache_output_attribute_vector.clear();
        cache_input_attribute_vector.clear();
        cache_custom_attribute_vector.clear();
        cache_system_attribute_vector.clear();
        
        ACULAPP_ << "Deallocate the user cache wrapper";
        if(attribute_shared_cache_wrapper) {
            delete(attribute_shared_cache_wrapper);
            attribute_shared_cache_wrapper = NULL;
        }
        
        //set healt to deinit
    } catch (CException& ex) {
        ACULDBG_ << "Exception on unit deinit:" << ex.what();
    }
    HealtManager::getInstance()->addNodeMetricValue(control_unit_id,
                                                    NodeHealtDefinitionKey::NODE_HEALT_STATUS,
                                                    NodeHealtDefinitionValue::NODE_HEALT_STATUS_DEINIT,
                                                    true);
    return NULL;
}


/*!
 Restore the control unit to a precise tag
 */
CDataWrapper* AbstractControlUnit::_unitRestoreToSnapshot(CDataWrapper *restoreParam,
                                                          bool& detachParam) throw(CException) {
    int err = 0;
    //check
    if(!restoreParam || !restoreParam->hasKey(NodeDomainAndActionRPC::ACTION_NODE_RESTORE_PARAM_TAG)) return NULL;
    
    if(getServiceState() != service_state_machine::InizializableServiceType::IS_INITIATED &&
       getServiceState() != service_state_machine::StartableServiceType::SS_STARTED ) {
        throw CException(-1, "Control Unit is not initilized or started", __PRETTY_FUNCTION__);
    }
    
    if(!key_data_storage) throw CException(-2, "Key data storage driver not allocated", __PRETTY_FUNCTION__);
    
    boost::shared_ptr<AttributeValueSharedCache> attribute_value_shared_cache( new AttributeValueSharedCache());
    if(!attribute_value_shared_cache.get()) throw CException(-3, "failed to allocate restore cache", __PRETTY_FUNCTION__);
    
    boost::shared_ptr<CDataWrapper> dataset_at_tag;
    //get tag alias
    const std::string restore_snapshot_tag = restoreParam->getStringValue(NodeDomainAndActionRPC::ACTION_NODE_RESTORE_PARAM_TAG);
    
    ACULDBG_ << "Start restoring snapshot tag for: " << restore_snapshot_tag;
    
    //load snapshot to restore
    if((err = key_data_storage->loadRestorePoint(restore_snapshot_tag))) {
        ACULERR_ << "Error loading dataset form snapshot tag: " << restore_snapshot_tag;
        throw CException(err, "Error loading dataset form snapshot", __PRETTY_FUNCTION__);
    } else {
        
        boost::shared_ptr<AttributeValueSharedCache> restore_cache(new AttributeValueSharedCache());
        restore_cache->init(NULL);
        
        for(int idx = 0; idx < 4; idx++) {
            //dataset loading sucessfull
            dataset_at_tag = key_data_storage->getDatasetFromRestorePoint(restore_snapshot_tag,
                                                                          (KeyDataStorageDomain)idx);
            if(dataset_at_tag.get()) {
                //fill cache with dataset key/value
                fillRestoreCacheWithDatasetFromTag((KeyDataStorageDomain)idx,
                                                   *dataset_at_tag.get(),
                                                   *restore_cache.get());
            }
        }
        
        try {
            //unitRestoreToSnapshot
            unitRestoreToSnapshot(restore_snapshot_tag,
                                  restore_cache.get());
        } catch (CException& ex) {
            DECODE_CHAOS_EXCEPTION(ex);
        }
        
        //end
        try {
            restore_cache->deinit();
        } catch (CException& ex) {
            DECODE_CHAOS_EXCEPTION(ex);
        }catch (...) {
            ACULERR_ << "General error on deinit restore cache";
        }
        
        //! clear snapshoted dataset to free memeory
        key_data_storage->clearRestorePoint(restore_snapshot_tag);
    }
    return NULL;
}

/*
 Receive the event for set the dataset input element
 */
CDataWrapper* AbstractControlUnit::_setDatasetAttribute(CDataWrapper *dataset_attribute_values,
                                                        bool& detachParam) throw (CException) {
    CDataWrapper *result = NULL;
    try {
        if(!dataset_attribute_values) {
            throw CException(-1, "No Input parameter", __PRETTY_FUNCTION__);
        }
        
        if(StartableService::getServiceState() == CUStateKey::DEINIT) {
            throw CException(-3, "The Control Unit is in deinit state", __PRETTY_FUNCTION__);
        }
        //send dataset attribute change pack to control unit implementation
        result = setDatasetAttribute(dataset_attribute_values, detachParam);
        
    } catch (CException& ex) {
        //at this time notify the wel gone setting of comand
        throw ex;
    }
    
    return result;
}

// Startable Service method
void AbstractControlUnit::init(void *init_data) throw(CException) {
    CDataWrapper *init_configuration = static_cast<CDataWrapper*>(init_data);
    if(!init_configuration ||
       !init_configuration->hasKey(NodeDefinitionKey::NODE_UNIQUE_ID)) {
        throw CException(-1, "No Device Init information in param", __PRETTY_FUNCTION__);
    }
    
    std::string deviceID = init_configuration->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    if(deviceID.compare(DatasetDB::getDeviceID())) {
        ACULERR_ << "device:" << deviceID << "not known by this Work Unit";
        throw CException(-2, "Device not known by this control unit", __PRETTY_FUNCTION__);
    }
    
    //cast to the CDatawrapper instance
    ACULAPP_ << "Initialize CU Database for device:" << deviceID;
    DatasetDB::addAttributeToDataSetFromDataWrapper(*init_configuration);
    
    //initialize key data storage for device id
    ACULAPP_ << "Create KeyDataStorage device:" << deviceID;
    key_data_storage = DataManager::getInstance()->getKeyDataStorageNewInstanceForKey(deviceID);
    
    ACULAPP_ << "Call KeyDataStorage init implementation for deviceID:" << deviceID;
    key_data_storage->init(init_configuration);
}

// Startable Service method
void AbstractControlUnit::start() throw(CException) {
    //init on shared cache the all the dataaset with the default value
    //set first timestamp for simulate the run step
    *timestamp_acq_cached_value->getValuePtr<uint64_t>() = TimingUtil::getTimeStamp();
    attribute_value_shared_cache->getSharedDomain(DOMAIN_OUTPUT).markAllAsChanged();
    pushOutputDataset();
    attribute_value_shared_cache->getSharedDomain(DOMAIN_INPUT).markAllAsChanged();
    pushInputDataset();
    attribute_value_shared_cache->getSharedDomain(DOMAIN_CUSTOM).markAllAsChanged();
    pushCustomDataset();
    attribute_value_shared_cache->getSharedDomain(DOMAIN_SYSTEM).markAllAsChanged();
    pushSystemDataset();
}

// Startable Service method
void AbstractControlUnit::stop() throw(CException) {
}

// Startable Service method
void AbstractControlUnit::deinit() throw(CException) {
    //remove key data storage
    if(key_data_storage) {
        ACULDBG_ << "Delete data storage driver for device:" << DatasetDB::getDeviceID();
        key_data_storage->deinit();
        delete(key_data_storage);
        key_data_storage = NULL;
    }
    
    
    
}

void AbstractControlUnit::fillCachedValueVector(AttributeCache& attribute_cache,
                                                std::vector<AttributeValue*>& cached_value) {
    for(int idx = 0;
        idx < attribute_cache.getNumberOfAttributes();
        idx++) {
        cached_value.push_back(attribute_cache.getValueSettingForIndex(idx));
    }
}

void AbstractControlUnit::initAttributeOnSharedAttributeCache(SharedCacheDomain domain,
                                                              std::vector<string>& attribute_names) {
    //add input attribute to shared setting
    CHAOS_ASSERT(attribute_value_shared_cache)
    RangeValueInfo attributeInfo;
    AttributeCache& attribute_setting = attribute_value_shared_cache->getSharedDomain(domain);
    
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
        try {
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
        } catch(boost::bad_lexical_cast const& e){
            ACULERR_ << e.what();
        }
        
    }
}

void AbstractControlUnit::completeOutputAttribute() {
    ACULDBG_ << "Complete the shared cache output attribute";
    AttributeCache& domain_attribute_setting = attribute_value_shared_cache->getSharedDomain(DOMAIN_OUTPUT);
    
    //add timestamp
    domain_attribute_setting.addAttribute(DataPackCommonKey::DPCK_TIMESTAMP, sizeof(uint64_t), DataType::TYPE_INT64);
    timestamp_acq_cached_value = domain_attribute_setting.getValueSettingForIndex(domain_attribute_setting.getIndexForName(DataPackCommonKey::DPCK_TIMESTAMP));
}

void AbstractControlUnit::completeInputAttribute() {
    
}

void AbstractControlUnit::initSystemAttributeOnSharedAttributeCache() {
    AttributeCache& domain_attribute_setting = attribute_value_shared_cache->getSharedDomain(DOMAIN_SYSTEM);
    
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
    
    //add error domain
    domain_attribute_setting.addAttribute(DataPackSystemKey::DP_SYS_LAST_ERROR_DOMAIN, 255, DataType::TYPE_STRING);
}

/*
 Get the current control unit state
 */
CDataWrapper* AbstractControlUnit::_getState(CDataWrapper* getStatedParam,
                                             bool& detachParam) throw(CException) {
    
    CDataWrapper *stateResult = new CDataWrapper();
    stateResult->addInt32Value(CUStateKey::CONTROL_UNIT_STATE, static_cast<CUStateKey::ControlUnitState>(StartableService::getServiceState()));
    return stateResult;
}

/*
 Get the current control unit state
 */
CDataWrapper* AbstractControlUnit::_getInfo(CDataWrapper* getStatedParam,
                                            bool& detachParam) throw(CException) {
    CDataWrapper *stateResult = new CDataWrapper();
    //set the string representing the type of the control unit
    stateResult->addStringValue(NodeDefinitionKey::NODE_TYPE, control_unit_type);
    return stateResult;
}

//!handler calledfor restor a control unit to a determinate point
void AbstractControlUnit::unitRestoreToSnapshot(const std::string& restore_snapshot_tag,
                                                AbstractSharedDomainCache * const restore_cache) throw(CException) {
    
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
    boost::shared_ptr<SharedCacheLockDomain> w_lock = attribute_value_shared_cache->getLockOnDomain(DOMAIN_INPUT, true);
    w_lock->lock();
    
    std::vector<std::string> in_attribute_name;
    RangeValueInfo attributeInfo;
    try {
        //call pre handler
        unitInputAttributePreChangeHandler();
        
        //get all input attribute name
        getDatasetAttributesName(DataType::Input , in_attribute_name);
        
        if(dataset_attribute_values->hasKey(NodeDefinitionKey::NODE_UNIQUE_ID)) {
            std::string node_id = dataset_attribute_values->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
            //compare the message device id and the local
            for (std::vector<std::string>::iterator iter = in_attribute_name.begin();
                 iter != in_attribute_name.end();
                 iter++) {
                //execute attribute handler
                const char * attr_name = iter->c_str();
                
                //check if the attribute name is present
                if(dataset_attribute_values->hasKey(attr_name)) {
                    
                    AttributeValue * attribute_cache_value = attribute_value_shared_cache->getAttributeValue(DOMAIN_INPUT, iter->c_str());
                    
                    //get attribute info
                    getAttributeRangeValueInfo(*iter, attributeInfo);
                    //call handler
                    switch (attribute_cache_value->type) {
                        case DataType::TYPE_BOOLEAN: {
                            bool bv = dataset_attribute_values->getBoolValue(attr_name);
                            attribute_cache_value->setValue(&bv, sizeof(bool));
                            break;
                        }
                        case DataType::TYPE_INT32: {
                            int32_t i32v = dataset_attribute_values->getInt32Value(attr_name);
                            CHECK_FOR_RANGE_VALUE(int32_t, i32v, attr_name)
                            attribute_cache_value->setValue(&i32v, sizeof(int32_t));
                            break;
                        }
                        case DataType::TYPE_INT64: {
                            int64_t i64v = dataset_attribute_values->getInt64Value(attr_name);
                            CHECK_FOR_RANGE_VALUE(int64_t, i64v, attr_name)
                            attribute_cache_value->setValue(&i64v, sizeof(int64_t));
                            break;
                        }
                        case DataType::TYPE_DOUBLE: {
                            double dv = dataset_attribute_values->getDoubleValue(attr_name);
                            CHECK_FOR_RANGE_VALUE(double, dv, attr_name)
                            attribute_cache_value->setValue(&dv, sizeof(double));
                            break;
                        }
                        case DataType::TYPE_STRING: {
                            std::string str = dataset_attribute_values->getStringValue(attr_name);
                            CHECK_FOR_STRING_RANGE_VALUE(str, attr_name)
                            attribute_cache_value->setValue(str.c_str(), (uint32_t)str.size());
                            break;
                        }
                        case DataType::TYPE_BYTEARRAY: {
                            int bin_size = 0;
                            const char *binv = dataset_attribute_values->getBinaryValue(attr_name, bin_size);
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
    if(!updatePack || !updatePack->hasKey(NodeDefinitionKey::NODE_UNIQUE_ID)) {
        throw CException(-1, "Update pack without DeviceID", __PRETTY_FUNCTION__);
    }
    
    string deviceID = updatePack->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    
    if(deviceID.compare(DatasetDB::getDeviceID())) {
        ACULAPP_ << "device:" << DatasetDB::getDeviceID() << "not known by this ContorlUnit";
        throw CException(-2, "Device not known by this control unit", __PRETTY_FUNCTION__);
    }
    
    //check to see if the device can ben initialized
    if(StartableService::getServiceState() == INIT_STATE) {
        ACULAPP_ << "device:" << DatasetDB::getDeviceID() << " not initialized";
        throw CException(-3, "Device Not Initilized", __PRETTY_FUNCTION__);
    }
    
    //check to see if the device can ben initialized
    if(key_data_storage) {
        key_data_storage->updateConfiguration(updatePack);
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
    AttributeCache& output_attribute_cache = attribute_value_shared_cache->getSharedDomain(DOMAIN_OUTPUT);
    boost::shared_ptr<SharedCacheLockDomain> r_lock = attribute_value_shared_cache->getLockOnDomain(DOMAIN_OUTPUT, false);
    r_lock->lock();
    
    //check if something as changed
    if(!output_attribute_cache.hasChanged()) return;
    
    CDataWrapper *output_attribute_dataset = key_data_storage->getNewOutputAttributeDataWrapper();
    if(!output_attribute_dataset) return;
    
    //write acq ts for second
    output_attribute_dataset->addInt64Value(timestamp_acq_cached_value->name.c_str(), *timestamp_acq_cached_value->getValuePtr<int64_t>());
    //add dataset type
    output_attribute_dataset->addInt32Value(DataPackCommonKey::DPCK_DATASET_TYPE, DataPackCommonKey::DPCK_DATASET_TYPE_OUTPUT);
    //add all other output channel
    for(int idx = 0;
        idx < cache_output_attribute_vector.size() - 1; //the device id and timestamp in added out of this list
        idx++) {
        //
        AttributeValue * value_set = cache_output_attribute_vector[idx];
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
    key_data_storage->pushDataSet(data_manager::KeyDataStorageDomainOutput, output_attribute_dataset);
    
    //reset chagned attribute into output dataset
    output_attribute_cache.resetChangedIndex();
}

//push system dataset
void AbstractControlUnit::pushInputDataset() {
    AttributeCache& input_attribute_cache = attribute_value_shared_cache->getSharedDomain(DOMAIN_INPUT);
    if(!input_attribute_cache.hasChanged()) return;
    //get the cdatawrapper for the pack
    CDataWrapper *input_attribute_dataset = key_data_storage->getNewOutputAttributeDataWrapper();
    if(input_attribute_dataset) {
        //input dataset timestamp is added only when pushed on cache
        input_attribute_dataset->addInt64Value(DataPackCommonKey::DPCK_TIMESTAMP, TimingUtil::getTimeStamp());
        
        //add dataset type
        input_attribute_dataset->addInt32Value(DataPackCommonKey::DPCK_DATASET_TYPE, DataPackCommonKey::DPCK_DATASET_TYPE_INPUT);
        
        //fill the dataset
        fillCDatawrapperWithCachedValue(cache_input_attribute_vector, *input_attribute_dataset);
        
        //push out the system dataset
        key_data_storage->pushDataSet(data_manager::KeyDataStorageDomainInput, input_attribute_dataset);
    }
}

//push system dataset
void AbstractControlUnit::pushCustomDataset() {
    AttributeCache& custom_attribute_cache = attribute_value_shared_cache->getSharedDomain(DOMAIN_CUSTOM);
    if(!custom_attribute_cache.hasChanged()) return;
    //get the cdatawrapper for the pack
    CDataWrapper *custom_attribute_dataset = key_data_storage->getNewOutputAttributeDataWrapper();
    if(custom_attribute_dataset) {
        //custom dataset timestamp is added only when pushed on cache
        custom_attribute_dataset->addInt64Value(DataPackCommonKey::DPCK_TIMESTAMP, TimingUtil::getTimeStamp());
        
        //add dataset type
        custom_attribute_dataset->addInt32Value(DataPackCommonKey::DPCK_DATASET_TYPE, DataPackCommonKey::DPCK_DATASET_TYPE_CUSTOM);
        
        //fill the dataset
        fillCDatawrapperWithCachedValue(cache_custom_attribute_vector, *custom_attribute_dataset);
        
        //push out the system dataset
        key_data_storage->pushDataSet(data_manager::KeyDataStorageDomainCustom, custom_attribute_dataset);
    }
}

void AbstractControlUnit::pushSystemDataset() {
    AttributeCache& systemm_attribute_cache = attribute_value_shared_cache->getSharedDomain(DOMAIN_SYSTEM);
    if(!systemm_attribute_cache.hasChanged()) return;
    //get the cdatawrapper for the pack
    CDataWrapper *system_attribute_dataset = key_data_storage->getNewOutputAttributeDataWrapper();
    if(system_attribute_dataset) {
        //system dataset timestamp is added when pushed on cache laso if contain the hearbeat field
        //! the dataaset can be pushed also in other moment
        system_attribute_dataset->addInt64Value(DataPackCommonKey::DPCK_TIMESTAMP, TimingUtil::getTimeStamp());
        //add dataset type
        system_attribute_dataset->addInt32Value(DataPackCommonKey::DPCK_DATASET_TYPE, DataPackCommonKey::DPCK_DATASET_TYPE_SYSTEM);
        //fill the dataset
        fillCDatawrapperWithCachedValue(cache_system_attribute_vector, *system_attribute_dataset);
        
        //push out the system dataset
        key_data_storage->pushDataSet(data_manager::KeyDataStorageDomainSystem, system_attribute_dataset);
    }
    //reset changed index
    systemm_attribute_cache.resetChangedIndex();
}

void AbstractControlUnit::fillCDatawrapperWithCachedValue(std::vector<AttributeValue*>& cached_attributes, CDataWrapper& dataset) {
    for(std::vector<AttributeValue*>::iterator it = cached_attributes.begin();
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

