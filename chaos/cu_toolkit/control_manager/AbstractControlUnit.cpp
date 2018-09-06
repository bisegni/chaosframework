/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */

#include <chaos/common/global.h>
#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/property/property.h>
#include <chaos/common/healt_system/HealtManager.h>
#include <chaos/common/event/channel/InstrumentEventChannel.h>

#include <chaos/cu_toolkit/data_manager/DataManager.h>
#include <chaos/cu_toolkit/driver_manager/DriverManager.h>
#include <chaos/cu_toolkit/command_manager/CommandManager.h>
#include <chaos/cu_toolkit/control_manager/AbstractControlUnit.h>

#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

using namespace boost::uuids;
using namespace chaos::common::data;
using namespace chaos::common::data::structured;
using namespace chaos::common::alarm;
using namespace chaos::common::utility;
using namespace chaos::common::property;
using namespace chaos::common::exception;
using namespace chaos::common::data::cache;
using namespace chaos::common::healt_system;
using namespace chaos::common::metadata_logging;

using namespace chaos::cu::data_manager;
using namespace chaos::cu::control_manager;
using namespace chaos::cu::driver_manager;
using namespace chaos::cu::driver_manager::driver;

#define ACULAPP_    LAPP_ << "[Control Unit:"<<control_unit_instance<<"-"<<control_unit_id<<"] -"
#define ACULNOTE_   LNOTE_ << "[Control Unit:"<<control_unit_instance<<"-"<<control_unit_id<<"] -"
#define ACULWRN_    LWRN_ << "[Control Unit:"<<control_unit_instance<<"-"<<control_unit_id<<"] -"
#define ACULDBG_    LDBG_ << "[Control Unit:"<<control_unit_instance<<"-"<<control_unit_id<<"] -"<<__FUNCTION__<<"-"
#define ACULERR_    LERR_ << "[Control Unit:"<<control_unit_instance<<"-"<<control_unit_id<<"](-"<<__FUNCTION__<<"-"<<__LINE__<<") - "

#define S(x) #x
#define S_(x) S(x)
#define S__LINE__ S_(__LINE__)

#define CHEK_IF_NEED_TO_THROW(flag, code) \
try{ \
code \
}catch(chaos::CException& ex){ \
ACULERR_ <<"CHAOS Exception on "<< DatasetDB::getDeviceID()<< ":\n"<<ex.what(); \
if(flag) throw chaos::common::exception::MetadataLoggingCException(getCUID(), ex.errorCode, ex.errorMessage, ex.errorDomain);\
}catch(...){\
ACULERR_ <<"Unknown exception on"<< DatasetDB::getDeviceID(); \
if(flag) throw chaos::common::exception::MetadataLoggingCException(getCUID(), -1000, S__LINE__, __PRETTY_FUNCTION__); \
}

#define GET_CAT_OR_EXIT(t,rv)\
if(map_variable_catalog.count(t) == 0) {return rv;}\
AlarmCatalog& catalog = map_variable_catalog[t];

#pragma mark StorageBurst
StorageBurst::StorageBurst(DatasetBurstShrdPtr _dataset_burst):
dataset_burst(_dataset_burst){}

StorageBurst::~StorageBurst(){}

#pragma mark PushStorageBurst
PushStorageBurst::PushStorageBurst(DatasetBurstShrdPtr _dataset_burst):
StorageBurst(MOVE(_dataset_burst)),
current_pushes(0){}

PushStorageBurst::~PushStorageBurst(){}

bool PushStorageBurst::active(void *data  __attribute__((unused))) {
    return ++current_pushes<StorageBurst::dataset_burst->value.asUInt32();
}

#pragma mark MSecStorageBurst
MSecStorageBurst::MSecStorageBurst(DatasetBurstShrdPtr _dataset_burst):
StorageBurst(MOVE(_dataset_burst)),
timeout_msec(TimingUtil::getTimestampWithDelay(StorageBurst::dataset_burst->value.asInt32(), true)){}

MSecStorageBurst::~MSecStorageBurst(){}

bool MSecStorageBurst::active(void *data) {
    int64_t *now = static_cast<int64_t*>(data);
    return timeout_msec>*now;
}

#pragma mark AbstractControlUnit
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
run_id(0),
standard_logging_channel(),
alarm_logging_channel(),
push_dataset_counter(0),
last_push_rate_grap_ts(0),
attribute_value_shared_cache(),
attribute_shared_cache_wrapper(),
use_custom_high_resolution_timestamp(false),
timestamp_acq_cached_value(),
timestamp_hw_acq_cached_value(),
thread_schedule_daly_cached_value(),
key_data_storage() {
    _initPropertyGroup();
    //!try to decode parameter string has json document
    is_control_unit_json_param = json_reader.parse(control_unit_param, json_parameter_document);
    //initialize check list
    _initChecklist();
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
run_id(0),
standard_logging_channel(),
alarm_logging_channel(),
push_dataset_counter(0),
last_push_rate_grap_ts(0),
attribute_value_shared_cache(),
attribute_shared_cache_wrapper(),
use_custom_high_resolution_timestamp(false),
timestamp_acq_cached_value(),
timestamp_hw_acq_cached_value(),
thread_schedule_daly_cached_value(),
key_data_storage() {
    _initPropertyGroup();
    //!try to decode parameter string has json document
    is_control_unit_json_param = json_reader.parse(control_unit_param, json_parameter_document);
    //copy array
    for (int idx = 0; idx < _control_unit_drivers.size(); idx++){
        control_unit_drivers.push_back(_control_unit_drivers[idx]);
    }
    
    //initialize check list
    _initChecklist();
}

void AbstractControlUnit::_initDrivers() throw(CException) {
    ACULAPP_ << "Initializating Driver Accessors";
    //at this point and before the unit implementation init i need to get
    //the infromation about the needed drivers
    std::vector<DrvRequestInfo> unit_needed_drivers;
    
    //got the needded driver definition
    unitDefineDriver(unit_needed_drivers);
    
    accessor_instances.clear();
    for (int idx = 0;
         idx != unit_needed_drivers.size();
         idx++) {
        driver_manager::driver::DriverAccessor *accessor_instance = driver_manager::DriverManager::getInstance()->getNewAccessorForDriverInstance(unit_needed_drivers[idx]);
        accessor_instances.push_back(accessor_instance);
    }
}

void AbstractControlUnit::_initChecklist() {
    //init checklists
    check_list_sub_service.addCheckList("_init");
    check_list_sub_service.getSharedCheckList("_init")->addElement(INIT_RPC_PHASE_CALL_INIT_STATE);
    check_list_sub_service.getSharedCheckList("_init")->addElement(INIT_RPC_PHASE_INIT_SHARED_CACHE);
    check_list_sub_service.getSharedCheckList("_init")->addElement(INIT_RPC_PHASE_COMPLETE_OUTPUT_ATTRIBUTE);
    check_list_sub_service.getSharedCheckList("_init")->addElement(INIT_RPC_PHASE_COMPLETE_INPUT_ATTRIBUTE);
    check_list_sub_service.getSharedCheckList("_init")->addElement(INIT_RPC_PHASE_INIT_SYSTEM_CACHE);
    check_list_sub_service.getSharedCheckList("_init")->addElement(INIT_RPC_PHASE_CALL_UNIT_DEFINE_ATTRIBUTE);
    check_list_sub_service.getSharedCheckList("_init")->addElement(INIT_RPC_PHASE_CREATE_FAST_ACCESS_CASCHE_VECTOR);
    check_list_sub_service.getSharedCheckList("_init")->addElement(INIT_RPC_PHASE_CALL_UNIT_INIT);
    check_list_sub_service.getSharedCheckList("_init")->addElement(INIT_RPC_PHASE_UPDATE_CONFIGURATION);
    check_list_sub_service.getSharedCheckList("_init")->addElement(INIT_RPC_PHASE_PUSH_DATASET);
    
    check_list_sub_service.addCheckList("init");
    check_list_sub_service.getSharedCheckList("init")->addElement(INIT_SM_PHASE_INIT_DB);
    check_list_sub_service.getSharedCheckList("init")->addElement(INIT_SM_PHASE_CREATE_DATA_STORAGE);
    
    
    check_list_sub_service.addCheckList("_start");
    check_list_sub_service.getSharedCheckList("_start")->addElement(START_RPC_PHASE_UNIT);
    check_list_sub_service.getSharedCheckList("_start")->addElement(START_RPC_PHASE_IMPLEMENTATION);
    
    check_list_sub_service.addCheckList("start");
    check_list_sub_service.getSharedCheckList("start")->addElement(START_SM_PHASE_STAT_TIMER);
}

void AbstractControlUnit::_initPropertyGroup() {
    PropertyGroup& pg_abstract_cu = addGroup(chaos::ControlUnitPropertyKey::GROUP_NAME);
    pg_abstract_cu.addProperty(ControlUnitDatapackSystemKey::BYPASS_STATE, "Put control unit in bypass state", DataType::TYPE_BOOLEAN, 0, CDataVariant((bool)false));
    pg_abstract_cu.addProperty(DataServiceNodeDefinitionKey::DS_STORAGE_TYPE, "Set the control unit storage type", DataType::TYPE_INT32, 0, CDataVariant((int32_t)0));
    pg_abstract_cu.addProperty(DataServiceNodeDefinitionKey::DS_STORAGE_LIVE_TIME, "Set the control unit storage type", DataType::TYPE_INT64, 0, CDataVariant((int64_t)0));
    pg_abstract_cu.addProperty(DataServiceNodeDefinitionKey::DS_STORAGE_HISTORY_TIME, "Set the control unit storage type", DataType::TYPE_INT64, 0, CDataVariant((int64_t)0));
    //    CDWUniquePtr burst_type_desc(new CDataWrapper());
    //    burst_type_desc->addInt32Value(DataServiceNodeDefinitionKey::DS_HISTORY_BURST_TYPE, DataServiceNodeDefinitionType::DSStorageBurstTypeUndefined);
    //    pg_abstract_cu.addProperty(DataServiceNodeDefinitionKey::DS_HISTORY_BURST, "Specify if the restore operation need to be done as real operation or not", DataType::TYPE_CLUSTER,0, CDataVariant(burst_type_desc.release()));
    
    pg_abstract_cu.addProperty(ControlUnitDatapackSystemKey::THREAD_SCHEDULE_DELAY, "Set the control unit step repeat time in microseconds", DataType::TYPE_INT64, 0, CDataVariant((int64_t)1000000));//set to one seconds
    pg_abstract_cu.addProperty(ControlUnitPropertyKey::INIT_RESTORE_OPTION, "Specify the restore type operatio to do durint initialization phase", DataType::TYPE_INT32, 0, CDataVariant((int32_t)0));
    pg_abstract_cu.addProperty(ControlUnitPropertyKey::INIT_RESTORE_APPLY, "Specify if the restore operation need to be done as real operation or not", DataType::TYPE_BOOLEAN,0, CDataVariant((bool)false));
    
    PropertyCollector::setPropertyValueChangeFunction(ChaosBind(&AbstractControlUnit::propertyChangeHandler, this,
                                                                ChaosBindPlaceholder(_1), ChaosBindPlaceholder(_2), ChaosBindPlaceholder(_3)));
    PropertyCollector::setPropertyValueUpdatedFunction(ChaosBind(&AbstractControlUnit::propertyUpdatedHandler, this,
                                                                 ChaosBindPlaceholder(_1), ChaosBindPlaceholder(_2), ChaosBindPlaceholder(_3), ChaosBindPlaceholder(_4)));
}

/*!
 Destructor a new CU with an identifier
 */
AbstractControlUnit::~AbstractControlUnit() {
    //clear the accessor of the driver
    for (int idx = 0;
         idx != accessor_instances.size();
         idx++) {
        driver_manager::DriverManager::getInstance()->releaseAccessor(accessor_instances[idx]);
    }
    //clear the vector
    accessor_instances.clear();
}

/*
 return the CU name
 */
const std::string& AbstractControlUnit::getCUInstance(){
    return control_unit_instance;
};

const std::string& AbstractControlUnit::getCUID() {
    return control_unit_id;
}

const std::string& AbstractControlUnit::getCUParam() {
    return control_unit_param;
}

const bool AbstractControlUnit::isCUParamInJson() {
    return is_control_unit_json_param;
}

const Json::Value& AbstractControlUnit::getCUParamJsonRootElement() {
    return json_parameter_document;
}

const std::string& AbstractControlUnit::getCUType() {
    return control_unit_type;
}

/*
 fill the CDataWrapper with AbstractCU system configuration, this method
 is called after getStartConfiguration directly by sandbox. in this method
 are defined the action for the input element of the dataset
 */
void AbstractControlUnit::_defineActionAndDataset(CDataWrapper& setup_configuration)  throw(CException) {
    
    vector<std::string> tempStringVector;
    
    if(control_unit_id.size()) {
        setDeviceID(control_unit_id);
    }
    
    //add the CU isntance, this can be redefinide by user in the unitDefineActionAndDataset method
    //for let the CU have the same instance at every run
    setup_configuration.addStringValue(NodeDefinitionKey::NODE_RPC_DOMAIN, control_unit_instance);
    
    //if we have a control key we attach it
    setup_configuration.addStringValue("mds_control_key", control_key);
    
    //add the control unit type with semantonc type::subtype
    setup_configuration.addStringValue(NodeDefinitionKey::NODE_TYPE, NodeType::NODE_TYPE_CONTROL_UNIT);
    setup_configuration.addStringValue(NodeDefinitionKey::NODE_SUB_TYPE, control_unit_type);
    //check if as been setuped a file for configuration
    //LCU_ << "Check if as been setup a json file path to configura CU:" << CU_IDENTIFIER_C_STREAM;
    //loadCDataWrapperForJsonFile(setup_configuration);
    
    
    //first call the setup abstract method used by the implementing CU to define action, dataset and other
    //usefull value
    unitDefineActionAndDataset();
    
    //call method to dinamically add other things to the dataset
    _completeDatasetAttribute();
    
    //for now we need only to add custom action for expose to rpc
    //input element of the dataset
    AbstActionDescShrPtr
    action_description = addActionDescritionInstance<AbstractControlUnit>(this,
                                                                          &AbstractControlUnit::_setDatasetAttribute,
                                                                          ControlUnitNodeDomainAndActionRPC::CONTROL_UNIT_APPLY_INPUT_DATASET_ATTRIBUTE_CHANGE_SET,
                                                                          "method for set the input element for the dataset");
    
    //expose updateConfiguration Methdo to rpc
    action_description = addActionDescritionInstance<AbstractControlUnit>(this,
                                                                          &AbstractControlUnit::updateConfiguration,
                                                                          NodeDomainAndActionRPC::ACTION_UPDATE_PROPERTY,
                                                                          "Update control unit property");
    
    action_description = addActionDescritionInstance<AbstractControlUnit>(this,
                                                                          &AbstractControlUnit::_init,
                                                                          NodeDomainAndActionRPC::ACTION_NODE_INIT,
                                                                          "Perform the control unit initialization");
    
    action_description = addActionDescritionInstance<AbstractControlUnit>(this,
                                                                          &AbstractControlUnit::_deinit,
                                                                          NodeDomainAndActionRPC::ACTION_NODE_DEINIT
                                                                          ,
                                                                          "Perform the control unit deinitialization");
    action_description = addActionDescritionInstance<AbstractControlUnit>(this,
                                                                          &AbstractControlUnit::_start,
                                                                          NodeDomainAndActionRPC::ACTION_NODE_START,
                                                                          "Start the control unit scheduling");
    
    action_description = addActionDescritionInstance<AbstractControlUnit>(this,
                                                                          &AbstractControlUnit::_stop,
                                                                          NodeDomainAndActionRPC::ACTION_NODE_STOP,
                                                                          "Stop the control unit scheduling");
    
    action_description = addActionDescritionInstance<AbstractControlUnit>(this,
                                                                          &AbstractControlUnit::_recover,
                                                                          NodeDomainAndActionRPC::ACTION_NODE_RECOVER,
                                                                          "Recovery a recoverable state, going to the last state");
    
    action_description = addActionDescritionInstance<AbstractControlUnit>(this,
                                                                          &AbstractControlUnit::_unitRestoreToSnapshot,
                                                                          NodeDomainAndActionRPC::ACTION_NODE_RESTORE,
                                                                          "Restore contorl unit to a snapshot tag");
    
    action_description = addActionDescritionInstance<AbstractControlUnit>(this,
                                                                          &AbstractControlUnit::_getState,
                                                                          NodeDomainAndActionRPC::ACTION_NODE_GET_STATE,
                                                                          "Get the state of the running control unit");
    
    action_description = addActionDescritionInstance<AbstractControlUnit>(this,
                                                                          &AbstractControlUnit::_getInfo,
                                                                          NodeDomainAndActionRPC::ACTION_CU_GET_INFO,
                                                                          "Get the information about running control unit");
    action_description = addActionDescritionInstance<AbstractControlUnit>(this,
                                                                          &AbstractControlUnit::_submitStorageBurst,
                                                                          ControlUnitNodeDomainAndActionRPC::ACTION_STORAGE_BURST,
                                                                          "Execute a storage burst on control unit");
    action_description->addParam(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_HISTORY_BURST_TAG, DataType::TYPE_STRING, "Tag associated to the stored data during burst");
    action_description->addParam(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_HISTORY_BURST_TYPE, DataType::TYPE_INT32, "The type of burst");
    action_description->addParam(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_HISTORY_BURST_VALUE, DataType::TYPE_UNDEFINED, "The value of the burst is defined by the type");
    
    action_description = addActionDescritionInstance<AbstractControlUnit>(this,
                                                                          &AbstractControlUnit::_datasetTagManagement,
                                                                          ControlUnitNodeDomainAndActionRPC::ACTION_DATASET_TAG_MANAGEMENT,
                                                                          "Execute a storage burst on control unit");
    action_description->addParam(ControlUnitNodeDomainAndActionRPC::ACTION_DATASET_TAG_MANAGEMENT_ADD_LIST, DataType::TYPE_ACCESS_ARRAY, "List of tag to be added to the control unit dataset");
    action_description->addParam(ControlUnitNodeDomainAndActionRPC::ACTION_DATASET_TAG_MANAGEMENT_REMOVE_LIST, DataType::TYPE_ACCESS_ARRAY, "List of tag to be removed to the control unit dataset");
    //grab dataset description
    DatasetDB::fillDataWrapperWithDataSetDescription(setup_configuration);
    
    //    //get action description
    //    getActionDescrionsInDataWrapper(setup_configuration);
    
    //add property description
    PropertyCollector::fillDescription("property", setup_configuration);
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


void AbstractControlUnit::_undefineActionAndDataset() throw(CException) {
    ACULDBG_ << "Remove Action Description";
    unitUndefineActionAndDataset();
}

void AbstractControlUnit::unitUndefineActionAndDataset() throw(CException) {
    
}

//! Get all managem declare action instance
void AbstractControlUnit::_getDeclareActionInstance(std::vector<const chaos::DeclareAction *>& declareActionInstance) {
    declareActionInstance.push_back(this);
}

//----------------------------------------- checklist method ------------------------------------------------
#pragma mark checklist method
void AbstractControlUnit::doInitRpCheckList() throw(CException) {
    std::vector<std::string> attribute_names;
    //rpc initialize service
    CHAOS_CHECK_LIST_START_SCAN_TO_DO(check_list_sub_service, "_init"){
        CHAOS_CHECK_LIST_DONE(check_list_sub_service, "_init", INIT_RPC_PHASE_CALL_INIT_STATE){
            //call init sequence
            init(NULL);
            break;
        }
        CHAOS_CHECK_LIST_DONE(check_list_sub_service, "_init", INIT_RPC_PHASE_INIT_SHARED_CACHE) {
            ACULAPP_ << "Allocate the user cache wrapper for:"+DatasetDB::getDeviceID();
            attribute_shared_cache_wrapper = new AttributeSharedCacheWrapper(attribute_value_shared_cache);
            
            ACULAPP_ << "Populating shared attribute cache for input attribute";
            DatasetDB::getDatasetAttributesName(DataType::Input, attribute_names);
            DatasetDB::getDatasetAttributesName(DataType::Bidirectional, attribute_names);
            initAttributeOnSharedAttributeCache(DOMAIN_INPUT, attribute_names);
            
            ACULAPP_ << "Populating shared attribute cache for output attribute";
            attribute_names.clear();
            DatasetDB::getDatasetAttributesName(DataType::Output, attribute_names);
            DatasetDB::getDatasetAttributesName(DataType::Bidirectional, attribute_names);
            initAttributeOnSharedAttributeCache(DOMAIN_OUTPUT, attribute_names);
            break;
        }
        
        CHAOS_CHECK_LIST_DONE(check_list_sub_service, "_init", INIT_RPC_PHASE_COMPLETE_OUTPUT_ATTRIBUTE){
            ACULAPP_ << "Complete shared attribute cache for output attribute";
            completeOutputAttribute();
            break;
        }
        CHAOS_CHECK_LIST_DONE(check_list_sub_service, "_init", INIT_RPC_PHASE_COMPLETE_INPUT_ATTRIBUTE){
            ACULAPP_ << "Complete shared attribute cache for input attribute";
            completeInputAttribute();
            break;
        }
        
        CHAOS_CHECK_LIST_DONE(check_list_sub_service, "_init", INIT_RPC_PHASE_INIT_SYSTEM_CACHE){
            ACULAPP_ << "Populating shared attribute cache for system attribute";
            initSystemAttributeOnSharedAttributeCache();
            break;
        }
        CHAOS_CHECK_LIST_DONE(check_list_sub_service, "_init", INIT_RPC_PHASE_CALL_UNIT_DEFINE_ATTRIBUTE){
            //define the implementations custom variable
            unitDefineCustomAttribute();
            break;
        }
        CHAOS_CHECK_LIST_DONE(check_list_sub_service, "_init", INIT_RPC_PHASE_CREATE_FAST_ACCESS_CASCHE_VECTOR){
            //create fast vector access for cached value
            fillCachedValueVector(attribute_value_shared_cache->getSharedDomain(DOMAIN_OUTPUT),
                                  cache_output_attribute_vector);
            
            fillCachedValueVector(attribute_value_shared_cache->getSharedDomain(DOMAIN_INPUT),
                                  cache_input_attribute_vector);
            
            fillCachedValueVector(attribute_value_shared_cache->getSharedDomain(DOMAIN_SYSTEM),
                                  cache_system_attribute_vector);
            
            fillCachedValueVector(attribute_value_shared_cache->getSharedDomain(DOMAIN_CUSTOM),
                                  cache_custom_attribute_vector);
            break;
        }
        CHAOS_CHECK_LIST_DONE(check_list_sub_service, "_init", INIT_RPC_PHASE_CALL_UNIT_INIT){
            //initialize implementations
            unitInit();
            break;
        }
        CHAOS_CHECK_LIST_DONE(check_list_sub_service, "_init", INIT_RPC_PHASE_UPDATE_CONFIGURATION){
            //call update param function
            updateConfiguration(MOVE(init_configuration->clone()));
            
            //check if we need to do a restore on first start
            PropertyGroupShrdPtr cug = PropertyCollector::getGroup(chaos::ControlUnitPropertyKey::GROUP_NAME);
            if(cug.get()) {
                if(cug->hasProperty(chaos::ControlUnitPropertyKey::INIT_RESTORE_APPLY) &&
                   cug->getProperty(chaos::ControlUnitPropertyKey::INIT_RESTORE_APPLY).getPropertyValue().isValid() &&
                   cug->getProperty(chaos::ControlUnitPropertyKey::INIT_RESTORE_APPLY).getPropertyValue().asBool()) {
                    //we need to add to stsart phase the restore one
                    check_list_sub_service.getSharedCheckList("_start")->addElement(START_RPC_PHASE_RESTORE_ON_FIRST_START);
                }
            }
            break;
        }
        CHAOS_CHECK_LIST_DONE(check_list_sub_service, "_init", INIT_RPC_PHASE_PUSH_DATASET){
            //init on shared cache the all the dataaset with the default value
            //set first timestamp for simulate the run step
            int err;
            *timestamp_acq_cached_value->getValuePtr<uint64_t>() = TimingUtil::getTimeStamp();
            attribute_value_shared_cache->getSharedDomain(DOMAIN_OUTPUT).markAllAsChanged();
            // if the CU can't push initial dataset is a real problem, we must detect immediately
            if((err=pushOutputDataset())!=0){
                throw CException(err,"cannot initialize output dataset",__PRETTY_FUNCTION__);
            }
            attribute_value_shared_cache->getSharedDomain(DOMAIN_INPUT).markAllAsChanged();
            
            if((err=pushInputDataset())!=0){
                throw CException(err,"cannot initialize input dataset",__PRETTY_FUNCTION__);
            }
            attribute_value_shared_cache->getSharedDomain(DOMAIN_CUSTOM).markAllAsChanged();
            
            if((err=pushCustomDataset())!=0){
                throw CException(err,"cannot initialize custom dataset",__PRETTY_FUNCTION__);
            }
            attribute_value_shared_cache->getSharedDomain(DOMAIN_SYSTEM).markAllAsChanged();
            
            if((err=pushSystemDataset())!=0){
                throw CException(err,"cannot initialize system dataset",__PRETTY_FUNCTION__);
            }
            
            if((err=pushCUAlarmDataset())!=0){
                throw CException(err,"cannot initialize CU alarm dataset",__PRETTY_FUNCTION__);
            }
            if((err=pushDevAlarmDataset())!=0){
                throw CException(err,"cannot initialize DEV alarm dataset",__PRETTY_FUNCTION__);
            }
            break;
        }
        
    }
    CHAOS_CHECK_LIST_END_SCAN_TO_DO(check_list_sub_service, "_init")
}
void AbstractControlUnit::doInitSMCheckList() throw(CException) {
    //rpc initialize service
    CHAOS_CHECK_LIST_START_SCAN_TO_DO(check_list_sub_service, "init"){
        CHAOS_CHECK_LIST_DONE(check_list_sub_service, "init", INIT_SM_PHASE_INIT_DB){
            
            //cast to the CDatawrapper instance
            ACULAPP_ << "Initialize CU Database for device:" << DatasetDB::getDeviceID();
            run_id = CDW_GET_INT64_WITH_DEFAULT(init_configuration, ControlUnitNodeDefinitionKey::CONTROL_UNIT_RUN_ID, 0);
            DatasetDB::addAttributeToDataSetFromDataWrapper(*init_configuration);
            break;
        }
        CHAOS_CHECK_LIST_DONE(check_list_sub_service, "init", INIT_SM_PHASE_CREATE_DATA_STORAGE) {
            //call init sequence
            //call update param function
            //initialize key data storage for device id
            ACULAPP_ << "Create KeyDataStorage device:" << DatasetDB::getDeviceID();
            key_data_storage.reset(DataManager::getInstance()->getKeyDataStorageNewInstanceForKey(DatasetDB::getDeviceID()));
            
            ACULAPP_ << "Call KeyDataStorage init implementation for deviceID:" << DatasetDB::getDeviceID();
            key_data_storage->init(init_configuration.get());
            break;
        }
    }
    CHAOS_CHECK_LIST_END_SCAN_TO_DO(check_list_sub_service, "init")
}

void AbstractControlUnit::doStartRpCheckList() throw(CException) {
    CHAOS_CHECK_LIST_START_SCAN_TO_DO(check_list_sub_service, "_start"){
        CHAOS_CHECK_LIST_DONE(check_list_sub_service, "_start", START_RPC_PHASE_IMPLEMENTATION){
            start();
            break;
        }
        CHAOS_CHECK_LIST_DONE(check_list_sub_service, "_start", START_RPC_PHASE_UNIT){
            unitStart();
            break;
        }
        CHAOS_CHECK_LIST_DONE(check_list_sub_service, "_start", START_RPC_PHASE_RESTORE_ON_FIRST_START) {
            try{
                checkForRestoreOnInit();
                check_list_sub_service.getSharedCheckList("_start")->removeElement(START_RPC_PHASE_RESTORE_ON_FIRST_START);
            } catch(CException& ex){
                check_list_sub_service.getSharedCheckList("_start")->removeElement(START_RPC_PHASE_RESTORE_ON_FIRST_START);
                throw;
            }
            break;
        }
    }
    CHAOS_CHECK_LIST_END_SCAN_TO_DO(check_list_sub_service, "_start")
}

void AbstractControlUnit::doStartSMCheckList() throw(CException) {
    CHAOS_CHECK_LIST_START_SCAN_TO_DO(check_list_sub_service, "start"){
        CHAOS_CHECK_LIST_DONE(check_list_sub_service, "start", START_SM_PHASE_STAT_TIMER){
            //register timer for push statistic
            chaos::common::async_central::AsyncCentralManager::getInstance()->addTimer(this, 0, chaos::common::constants::CUTimersTimeoutinMSec);
            //get timestamp for first pushes metric acquisition
            last_push_rate_grap_ts = TimingUtil::getTimeStamp();
        }
    }
    CHAOS_CHECK_LIST_END_SCAN_TO_DO(check_list_sub_service, "start")
}

void AbstractControlUnit::redoInitRpCheckList(bool throw_exception) throw(CException) {
    //rpc initialize service
    CHAOS_CHECK_LIST_START_SCAN_DONE(check_list_sub_service, "_init"){
        CHAOS_CHECK_LIST_REDO(check_list_sub_service, "_init", INIT_RPC_PHASE_CALL_INIT_STATE){
            //saftely deinititalize the abstract control unit
            CHEK_IF_NEED_TO_THROW(throw_exception, deinit();)
            break;
        }
        CHAOS_CHECK_LIST_REDO(check_list_sub_service, "_init", INIT_RPC_PHASE_INIT_SHARED_CACHE) {
            ACULAPP_ << "Deallocate the user cache wrapper for:"+DatasetDB::getDeviceID();
            if(attribute_shared_cache_wrapper) {
                delete(attribute_shared_cache_wrapper);
                attribute_shared_cache_wrapper = NULL;
            }
            break;
        }
        CHAOS_CHECK_LIST_REDO(check_list_sub_service, "_init", INIT_RPC_PHASE_COMPLETE_OUTPUT_ATTRIBUTE){
            break;
        }
        CHAOS_CHECK_LIST_REDO(check_list_sub_service, "_init", INIT_RPC_PHASE_COMPLETE_INPUT_ATTRIBUTE){
            break;
        }
        CHAOS_CHECK_LIST_REDO(check_list_sub_service, "_init", INIT_RPC_PHASE_INIT_SYSTEM_CACHE){
            break;
        }
        CHAOS_CHECK_LIST_REDO(check_list_sub_service, "_init", INIT_RPC_PHASE_CALL_UNIT_DEFINE_ATTRIBUTE){
            CHEK_IF_NEED_TO_THROW(throw_exception, unitDefineCustomAttribute();)
            break;
        }
        CHAOS_CHECK_LIST_REDO(check_list_sub_service, "_init", INIT_RPC_PHASE_CREATE_FAST_ACCESS_CASCHE_VECTOR){
            //clear all cache sub_structure
            CHEK_IF_NEED_TO_THROW(throw_exception,
                                  cache_output_attribute_vector.clear();
                                  cache_input_attribute_vector.clear();
                                  cache_custom_attribute_vector.clear();
                                  cache_system_attribute_vector.clear();)
            break;
        }
        CHAOS_CHECK_LIST_REDO(check_list_sub_service, "_init", INIT_RPC_PHASE_CALL_UNIT_INIT){
            ACULDBG_ << "Deinit custom deinitialization for device:" << DatasetDB::getDeviceID();
            CHEK_IF_NEED_TO_THROW(throw_exception, unitDeinit();)
            break;
        }
        CHAOS_CHECK_LIST_REDO(check_list_sub_service, "_init", INIT_RPC_PHASE_UPDATE_CONFIGURATION) {
            break;
        }
        CHAOS_CHECK_LIST_REDO(check_list_sub_service, "_init", INIT_RPC_PHASE_PUSH_DATASET){
            break;
        }
    }
    CHAOS_CHECK_LIST_END_SCAN_DONE(check_list_sub_service, "_init")
}

void AbstractControlUnit::redoInitSMCheckList(bool throw_exception) throw(CException) {
    CHAOS_CHECK_LIST_START_SCAN_DONE(check_list_sub_service, "init"){
        CHAOS_CHECK_LIST_REDO(check_list_sub_service, "init",  INIT_SM_PHASE_INIT_DB){
            break;
        }
        CHAOS_CHECK_LIST_REDO(check_list_sub_service, "init", INIT_SM_PHASE_CREATE_DATA_STORAGE) {
            //remove key data storage
            CHEK_IF_NEED_TO_THROW(throw_exception,
                                  if(key_data_storage.get()) {
                                      ACULDBG_ << "Delete data storage driver for device:" << DatasetDB::getDeviceID();
                                      key_data_storage->deinit();
                                      key_data_storage.reset();
                                  }
                                  )
            break;
        }
    }
    CHAOS_CHECK_LIST_END_SCAN_DONE(check_list_sub_service, "init")
}

void AbstractControlUnit::redoStartRpCheckList(bool throw_exception) throw(CException) {
    CHAOS_CHECK_LIST_START_SCAN_DONE(check_list_sub_service, "_start"){
        CHAOS_CHECK_LIST_REDO(check_list_sub_service, "_start", START_RPC_PHASE_IMPLEMENTATION){
            CHEK_IF_NEED_TO_THROW(throw_exception, stop();)
            break;
        }
        //unit sto need to go after the abstract cu has been stopped
        CHAOS_CHECK_LIST_REDO(check_list_sub_service, "_start", START_RPC_PHASE_UNIT){
            CHEK_IF_NEED_TO_THROW(throw_exception, unitStop();)
        }
    }
    CHAOS_CHECK_LIST_END_SCAN_DONE(check_list_sub_service, "_start")
}

void AbstractControlUnit::redoStartSMCheckList(bool throw_exception) throw(CException) {
    CHAOS_CHECK_LIST_START_SCAN_DONE(check_list_sub_service, "start"){
        CHAOS_CHECK_LIST_REDO(check_list_sub_service, "start", START_SM_PHASE_STAT_TIMER){
            //remove timer for push statistic
            CHEK_IF_NEED_TO_THROW(throw_exception, chaos::common::async_central::AsyncCentralManager::getInstance()->removeTimer(this);)
        }
    }
    CHAOS_CHECK_LIST_END_SCAN_DONE(check_list_sub_service, "start")
}


//----------------------------------------- protected initi/deinit method ------------------------------------------------
#pragma mark RPC State Machine method
CDWUniquePtr AbstractControlUnit::_init(CDWUniquePtr init_configuration) {
    if(getServiceState() == CUStateKey::INIT) {
        return CDWUniquePtr();
    }
    
    try {
        if(getServiceState() == CUStateKey::RECOVERABLE_ERROR) {
            LOG_AND_TROW_FORMATTED(ACULERR_, -2, "Recoverable error state need to be recovered for %1%", %__PRETTY_FUNCTION__)
        }
        
        //if(getServiceState() != CUStateKey::DEINIT) throw CException(-1, DatasetDB::getDeviceID()+" need to be in deinit", __PRETTY_FUNCTION__);
        if(!attribute_value_shared_cache) {LOG_AND_TROW_FORMATTED(ACULERR_, -3, "No Shared cache implementation found for %1%[%2%]", %DatasetDB::getDeviceID()%__PRETTY_FUNCTION__);}
        
        if(!SWEService::initImplementation(this, "AbstractControlUnit", __PRETTY_FUNCTION__)) {
            LOG_AND_TROW_FORMATTED(ACULERR_, -1, "Control Unit %1% can't be initilized [state mismatch]!", %DatasetDB::getDeviceID());
        }
        
    } catch (MetadataLoggingCException& ex) {
        throw;
    } catch (CException& ex) {
        throw MetadataLoggingCException(getCUID(),
                                        ex.errorCode,
                                        ex.errorMessage,
                                        ex.errorDomain);
    }
    
    try {
        
        //update configuraiton and own it
        this->init_configuration = MOVE(init_configuration);
        
        HealtManager::getInstance()->addNodeMetricValue(control_unit_id,
                                                        NodeHealtDefinitionKey::NODE_HEALT_STATUS,
                                                        NodeHealtDefinitionValue::NODE_HEALT_STATUS_INITING,
                                                        true);
        doInitRpCheckList();
        
        //set healt to init
        HealtManager::getInstance()->addNodeMetricValue(control_unit_id,
                                                        NodeHealtDefinitionKey::NODE_HEALT_STATUS,
                                                        NodeHealtDefinitionValue::NODE_HEALT_STATUS_INIT,
                                                        true);
    } catch (MetadataLoggingCException& ex) {
        SWEService::goInFatalError(this,
                                   ex,
                                   "AbstractControlUnit",
                                   __PRETTY_FUNCTION__);
        throw;
    } catch(CException& ex) {
        MetadataLoggingCException loggable_exception(getCUID(),
                                                     ex.errorCode,
                                                     ex.errorMessage,
                                                     ex.errorDomain);
        //go in falta error
        SWEService::goInFatalError(this,
                                   loggable_exception,
                                   "AbstractControlUnit",
                                   __PRETTY_FUNCTION__);
        throw loggable_exception;
    }
    return CDWUniquePtr();
}

CDWUniquePtr AbstractControlUnit::_start(CDWUniquePtr startParam){
    if(getServiceState() == CUStateKey::START) {
        return CDWUniquePtr();
    }
    try {
        
        if(getServiceState() == CUStateKey::RECOVERABLE_ERROR) {
            LOG_AND_TROW_FORMATTED(ACULERR_, -1, "Recoverable error state need to be recovered for %1%", %__PRETTY_FUNCTION__)
        }
        //call start method of the startable interface
        if(!SWEService::startImplementation(this, "AbstractControlUnit", __PRETTY_FUNCTION__)) {
            LOG_AND_TROW_FORMATTED(ACULERR_, -1, "Control Unit %1% can't be started [state mismatch]!", %DatasetDB::getDeviceID());
        }
        
    }  catch (MetadataLoggingCException& ex) {
        throw;
    } catch (CException& ex) {
        throw MetadataLoggingCException(getCUID(),
                                        ex.errorCode,
                                        ex.errorMessage,
                                        ex.errorDomain);
    }
    
    try {
        
        HealtManager::getInstance()->addNodeMetricValue(control_unit_id,
                                                        NodeHealtDefinitionKey::NODE_HEALT_STATUS,
                                                        NodeHealtDefinitionValue::NODE_HEALT_STATUS_STARTING,
                                                        true);
        
        doStartRpCheckList();
        
        //set healt to start
        HealtManager::getInstance()->addNodeMetricValue(control_unit_id,
                                                        NodeHealtDefinitionKey::NODE_HEALT_STATUS,
                                                        NodeHealtDefinitionValue::NODE_HEALT_STATUS_START,
                                                        true);
    } catch (MetadataLoggingCException& ex) {
        SWEService::goInFatalError(this,
                                   ex,
                                   "AbstractControlUnit",
                                   __PRETTY_FUNCTION__);
        throw;
    } catch(CException& ex) {
        MetadataLoggingCException loggable_exception(getCUID(),
                                                     ex.errorCode,
                                                     ex.errorMessage,
                                                     ex.errorDomain);
        //go in falta error
        SWEService::goInFatalError(this, loggable_exception, "AbstractControlUnit", __PRETTY_FUNCTION__);
        throw loggable_exception;
    }
    return CDWUniquePtr();
}

CDWUniquePtr AbstractControlUnit::_stop(CDWUniquePtr stopParam){
    if(getServiceState() == CUStateKey::STOP) {
        return CDWUniquePtr();
    }
    try {
        /*
         * stop transition is possible with recoverable error
         *  if(getServiceState() == CUStateKey::RECOVERABLE_ERROR) {
         LOG_AND_TROW_FORMATTED(ACULERR_, -1, "Recoverable error state need to be recovered for %1%", %__PRETTY_FUNCTION__)
         }
         */
        //first we start the deinitializaiton of the implementation unit
        if(!SWEService::stopImplementation(this, "AbstractControlUnit", __PRETTY_FUNCTION__)) {
            LOG_AND_TROW_FORMATTED(ACULERR_, -1, "Control Unit %1% can't be stopped [state mismatch]!", %DatasetDB::getDeviceID());
        }
    }  catch (MetadataLoggingCException& ex) {
        SWEService::goInFatalError(this,
                                   ex,
                                   "AbstractControlUnit",
                                   __PRETTY_FUNCTION__);
        throw;
    } catch (CException& ex) {
        throw MetadataLoggingCException(getCUID(),
                                        ex.errorCode,
                                        ex.errorMessage,
                                        ex.errorDomain);
    }
    
    try {
        //set healt to start
        HealtManager::getInstance()->addNodeMetricValue(control_unit_id,
                                                        NodeHealtDefinitionKey::NODE_HEALT_STATUS,
                                                        NodeHealtDefinitionValue::NODE_HEALT_STATUS_STOPING,
                                                        true);
        redoStartRpCheckList();
        
        HealtManager::getInstance()->addNodeMetricValue(control_unit_id,
                                                        NodeHealtDefinitionKey::NODE_HEALT_STATUS,
                                                        NodeHealtDefinitionValue::NODE_HEALT_STATUS_STOP,
                                                        true);
    }  catch (MetadataLoggingCException& ex) {
        SWEService::goInFatalError(this,
                                   ex,
                                   "AbstractControlUnit",
                                   __PRETTY_FUNCTION__);
        throw;
    } catch (CException& ex) {
        MetadataLoggingCException loggable_exception(getCUID(),
                                                     ex.errorCode,
                                                     ex.errorMessage,
                                                     ex.errorDomain);
        //go in falta error
        SWEService::goInFatalError(this, loggable_exception, "AbstractControlUnit", __PRETTY_FUNCTION__);
        throw loggable_exception;
    }
    
    return CDWUniquePtr();
}


/*go
 deinit all datastorage
 */
CDWUniquePtr AbstractControlUnit::_deinit(CDWUniquePtr deinitParam) {
    if(getServiceState() == CUStateKey::DEINIT) {
        return CDWUniquePtr();
    }
    try {
        /*
         * deinit transition is possible with recoverable error
         *  if(getServiceState() == CUStateKey::RECOVERABLE_ERROR) {
         LOG_AND_TROW_FORMATTED(ACULERR_, -1, "Recoverable error state need to be recovered for %1%", %__PRETTY_FUNCTION__)
         }
         */
        if(!SWEService::deinitImplementation(this, "AbstractControlUnit", __PRETTY_FUNCTION__)) {
            LOG_AND_TROW_FORMATTED(ACULERR_, -1, "Control Unit %1% can't be deinitilized [state mismatch]!", %DatasetDB::getDeviceID());
        }
    }  catch (MetadataLoggingCException& ex) {
        throw;
    }  catch (CException& ex) {
        throw MetadataLoggingCException(getCUID(),
                                        ex.errorCode,
                                        ex.errorMessage,
                                        ex.errorDomain);
    }
    
    //first we start the deinitializaiton of the implementation unit
    try {
        HealtManager::getInstance()->addNodeMetricValue(control_unit_id,
                                                        NodeHealtDefinitionKey::NODE_HEALT_STATUS,
                                                        NodeHealtDefinitionValue::NODE_HEALT_STATUS_DEINITING,
                                                        true);
        
        redoInitRpCheckList();
        
        //set healt to deinit
        HealtManager::getInstance()->addNodeMetricValue(control_unit_id,
                                                        NodeHealtDefinitionKey::NODE_HEALT_STATUS,
                                                        NodeHealtDefinitionValue::NODE_HEALT_STATUS_DEINIT,
                                                        true);
    }  catch (MetadataLoggingCException& ex) {
        SWEService::goInFatalError(this,
                                   ex,
                                   "AbstractControlUnit",
                                   __PRETTY_FUNCTION__);
        throw;
    } catch (CException& ex) {
        MetadataLoggingCException loggable_exception(getCUID(),
                                                     ex.errorCode,
                                                     ex.errorMessage,
                                                     ex.errorDomain);
        
        //go in falta error
        SWEService::goInFatalError(this, loggable_exception, "AbstractControlUnit", __PRETTY_FUNCTION__);
        throw loggable_exception;
    }
    
    return CDWUniquePtr();
}

CDWUniquePtr AbstractControlUnit::_recover(CDWUniquePtr gdeinitParam) {
    if(getServiceState() != CUStateKey::RECOVERABLE_ERROR) throw MetadataLoggingCException(getCUID(), -1, DatasetDB::getDeviceID()+" need to be recoverable errore in the way to be recoverable!", __PRETTY_FUNCTION__);
    
    //first we start the deinitializaiton of the implementation unit
    try {
        if(SWEService::recoverError(this, "AbstractControlUnit", __PRETTY_FUNCTION__)) {
            
        } else {
            
        }
    } catch (MetadataLoggingCException& ex) {
        SWEService::goInFatalError(this,
                                   ex,
                                   "AbstractControlUnit",
                                   __PRETTY_FUNCTION__);
        throw;
    }  catch (CException& ex) {
        MetadataLoggingCException loggable_exception(getCUID(),
                                                     ex.errorCode,
                                                     ex.errorMessage,
                                                     ex.errorDomain);
        //go in falta error
        SWEService::goInFatalError(this, loggable_exception, "AbstractControlUnit", __PRETTY_FUNCTION__);
        throw loggable_exception;
    }
    
    return CDWUniquePtr();
}

//! fill cache with found dataset at the restore point
void AbstractControlUnit::fillRestoreCacheWithDatasetFromTag(data_manager::KeyDataStorageDomain domain,
                                                             CDataWrapper& dataset,
                                                             AbstractSharedDomainCache& restore_cache) {
    std::vector<std::string> dataset_key;
    dataset.getAllKey(dataset_key);
    for(std::vector<std::string>::iterator it = dataset_key.begin();
        it != dataset_key.end();
        it++) {
        restore_cache.addAttribute((SharedCacheDomain)domain,
                                   *it,
                                   dataset.getVariantValue(*it));
    }
}

void AbstractControlUnit::checkForRestoreOnInit()  throw(CException) {
    //now we can launch the restore the current input attrite, remeber that
    //input attribute are composed by mds so the type of restore data(static conf or live) is manage at mds leve
    //control unit in case off pply true need only to launch the restore on current input dataset set.
    try {
        ChaosUniquePtr<AttributeValueSharedCache> restore_cache(new AttributeValueSharedCache());
        if(!restore_cache.get()) throw MetadataLoggingCException(getCUID(), -3, "failed to allocate restore cache", __PRETTY_FUNCTION__);
        
        AttributeCache& ac_src = attribute_value_shared_cache->getSharedDomain(DOMAIN_INPUT);
        AttributeCache& ac_dst = restore_cache->getSharedDomain(DOMAIN_INPUT);
        ac_src.copyToAttributeCache(ac_dst);
        
        //unitRestoreToSnapshot
        if(unitRestoreToSnapshot(restore_cache.get())){
            metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelInfo,
                            "Restore to initilization value run successfully");
        } else {
            metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,
                            "Restore to initilization value has fault");
            
            //input attribute already already updated
        }
    } catch (MetadataLoggingCException& ex) {
        throw;
    }  catch (CException& ex) {
        MetadataLoggingCException loggable_exception(getCUID(),
                                                     ex.errorCode,
                                                     ex.errorMessage,
                                                     ex.errorDomain);
        
        DECODE_CHAOS_EXCEPTION(loggable_exception);
    }
}

/*!
 Restore the control unit to a precise tag
 */
CDWUniquePtr AbstractControlUnit::_unitRestoreToSnapshot(CDWUniquePtr restoreParam) {
    int err = 0;
    //check
    if(!restoreParam.get() || !restoreParam->hasKey(NodeDomainAndActionRPC::ACTION_NODE_RESTORE_PARAM_TAG)) return CDWUniquePtr();
    
    if(getServiceState() != CUStateKey::START ) {
        throw MetadataLoggingCException(getCUID(), -1, "Control Unit restore can appen only in start state", __PRETTY_FUNCTION__);
    }
    
    if(!key_data_storage.get()) throw MetadataLoggingCException(getCUID(), -2, "Key data storage driver not allocated", __PRETTY_FUNCTION__);
    
    ChaosSharedPtr<AttributeValueSharedCache> restore_cache(new AttributeValueSharedCache());
    if(!restore_cache.get()) throw MetadataLoggingCException(getCUID(), -3, "failed to allocate restore cache", __PRETTY_FUNCTION__);
    
    ChaosSharedPtr<CDataWrapper> dataset_at_tag;
    //get tag alias
    const std::string restore_snapshot_tag = restoreParam->getStringValue(NodeDomainAndActionRPC::ACTION_NODE_RESTORE_PARAM_TAG);
    
    metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelInfo,
                    CHAOS_FORMAT("Start restoring snapshot tag for: %1%", %restore_snapshot_tag));
    
    //load snapshot to restore
    if((err = key_data_storage->loadRestorePoint(restore_snapshot_tag))) {
        ACULERR_ << "Error loading dataset form snapshot tag: " << restore_snapshot_tag;
        throw MetadataLoggingCException(getCUID(), err, "Error loading dataset form snapshot", __PRETTY_FUNCTION__);
    } else {
        
        restore_cache->init(NULL);
        
        for(int idx = 0; idx < 4; idx++) {
            //dataset loading sucessfull
            dataset_at_tag = key_data_storage->getDatasetFromRestorePoint(restore_snapshot_tag,
                                                                          (KeyDataStorageDomain)idx);
            if(dataset_at_tag.get()) {
                ACULDBG_ << CHAOS_FORMAT("Dataset restored from tag %1% -> %2%",%restore_snapshot_tag%dataset_at_tag->getJSONString());
                //fill cache with dataset key/value
                fillRestoreCacheWithDatasetFromTag((KeyDataStorageDomain)idx,
                                                   *dataset_at_tag.get(),
                                                   *restore_cache.get());
            }
        }
        
        try {
            //unitRestoreToSnapshot
            if(unitRestoreToSnapshot(restore_cache.get())){
                metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelInfo,
                                CHAOS_FORMAT("Restore for %1% has been run successfully", %restore_snapshot_tag));
            } else {
                metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,
                                CHAOS_FORMAT("Restore for %1% has been faulted", %restore_snapshot_tag));
                
                //input dataset need to be update
                AttributeCache& ac_src = restore_cache->getSharedDomain(DOMAIN_INPUT);
                AttributeCache& ac_dst = attribute_value_shared_cache->getSharedDomain(DOMAIN_INPUT);
                ac_src.copyToAttributeCache(ac_dst);
                
            }
        } catch (MetadataLoggingCException& ex) {
            throw;
        }  catch (CException& ex) {
            MetadataLoggingCException loggable_exception(getCUID(),
                                                         ex.errorCode,
                                                         ex.errorMessage,
                                                         ex.errorDomain);
            
            DECODE_CHAOS_EXCEPTION(loggable_exception);
        }
        
        //end
        try {
            restore_cache->deinit();
        }  catch (MetadataLoggingCException& ex) {
            throw;
        } catch (CException& ex) {
            MetadataLoggingCException loggable_exception(getCUID(),
                                                         ex.errorCode,
                                                         ex.errorMessage,
                                                         ex.errorDomain);
            DECODE_CHAOS_EXCEPTION(loggable_exception);
        }catch (...) {
            ACULERR_ << "General error on deinit restore cache";
        }
        
        //! clear snapshoted dataset to free memeory
        key_data_storage->clearRestorePoint(restore_snapshot_tag);
    }
    metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelInfo,
                    CHAOS_FORMAT("End restoring snapshot tag for: %1%", %restore_snapshot_tag));
    return CDWUniquePtr();
}

/*
 Receive the event for set the dataset input element
 */
CDWUniquePtr AbstractControlUnit::_setDatasetAttribute(CDWUniquePtr dataset_attribute_values) {
    CDWUniquePtr result;
    try {
        if(!dataset_attribute_values.get()) {
            throw MetadataLoggingCException(getCUID(), -1, "No Input parameter", __PRETTY_FUNCTION__);
        }
        if(SWEService::getServiceState() == CUStateKey::DEINIT) {
            throw MetadataLoggingCException(getCUID(), -3, "The Control Unit is in deinit state", __PRETTY_FUNCTION__);
        }
        //send dataset attribute change pack to control unit implementation
        result = setDatasetAttribute(MOVE(dataset_attribute_values));
    } catch (CException& ex) {
        throw;
    }
    return result;
}
#pragma mark State Machine method
// Startable Service method
void AbstractControlUnit::init(void *init_data) throw(CException) {
    //allocate metadata loggin channel for alarm
    alarm_logging_channel = (AlarmLoggingChannel*)MetadataLoggingManager::getInstance()->getChannel("AlarmLoggingChannel");
    if(alarm_logging_channel == NULL) {LOG_AND_TROW(ACULERR_, -1, "Alarm logging channel not found");}
    
    standard_logging_channel = (StandardLoggingChannel*)MetadataLoggingManager::getInstance()->getChannel("StandardLoggingChannel");
    if(standard_logging_channel == NULL) {LOG_AND_TROW(ACULERR_, -2, "Standard logging channel not found");}
    standard_logging_channel->setLogLevel(common::metadata_logging::StandardLoggingChannel::LogLevelInfo);
    //the init of the implementation unit goes after the infrastructure one
    doInitSMCheckList();
}

// Startable Service method
void AbstractControlUnit::start() throw(CException) {
    doStartSMCheckList();
}

// Startable Service method
void AbstractControlUnit::stop() throw(CException) {
    redoStartSMCheckList();
}

// Startable Service method
void AbstractControlUnit::deinit() throw(CException) {
    redoInitSMCheckList();
    if(alarm_logging_channel) {
        MetadataLoggingManager::getInstance()->releaseChannel(alarm_logging_channel);
        alarm_logging_channel = NULL;
    }
    
    if(standard_logging_channel) {
        MetadataLoggingManager::getInstance()->releaseChannel(standard_logging_channel);
        standard_logging_channel = NULL;
    }
}

//! State machine is gone into recoverable error
void AbstractControlUnit::recoverableErrorFromState(int last_state, chaos::CException& ex) {
    ACULERR_ << "recoverableErrorFromState with state:" << last_state;
    
    //update healt tstatus to report recoverable error
    HealtManager::getInstance()->addNodeMetricValue(control_unit_id,
                                                    NodeHealtDefinitionKey::NODE_HEALT_STATUS,
                                                    NodeHealtDefinitionValue::NODE_HEALT_STATUS_RERROR,
                                                    false);
    HealtManager::getInstance()->addNodeMetricValue(control_unit_id,
                                                    NodeHealtDefinitionKey::NODE_HEALT_LAST_ERROR_CODE,
                                                    ex.errorCode,
                                                    false);
    HealtManager::getInstance()->addNodeMetricValue(control_unit_id,
                                                    NodeHealtDefinitionKey::NODE_HEALT_LAST_ERROR_MESSAGE,
                                                    ex.errorMessage,
                                                    false);
    HealtManager::getInstance()->addNodeMetricValue(control_unit_id,
                                                    NodeHealtDefinitionKey::NODE_HEALT_LAST_ERROR_DOMAIN,
                                                    ex.errorDomain,
                                                    true);
    //we stop the run action
    CHAOS_NOT_THROW(stop();)
}

//! State machine is gone into recoverable error
bool AbstractControlUnit::beforeRecoverErrorFromState(int last_state) {
    ACULERR_ << "beforeRecoverErrorFromState with state:" << last_state;
    std::string last_state_str;
    switch(last_state) {
        case CUStateKey::INIT:
            last_state_str = NodeHealtDefinitionValue::NODE_HEALT_STATUS_INIT;
            break;
        case CUStateKey::DEINIT:
            last_state_str = NodeHealtDefinitionValue::NODE_HEALT_STATUS_DEINIT;
            break;
        case CUStateKey::START:
            last_state_str = NodeHealtDefinitionValue::NODE_HEALT_STATUS_START;
            break;
        case CUStateKey::STOP:
            last_state_str = NodeHealtDefinitionValue::NODE_HEALT_STATUS_STOP;
            break;
    }
    HealtManager::getInstance()->addNodeMetricValue(control_unit_id,
                                                    NodeHealtDefinitionKey::NODE_HEALT_STATUS,
                                                    last_state_str,
                                                    false);
    HealtManager::getInstance()->addNodeMetricValue(control_unit_id,
                                                    NodeHealtDefinitionKey::NODE_HEALT_LAST_ERROR_CODE,
                                                    0,
                                                    false);
    HealtManager::getInstance()->addNodeMetricValue(control_unit_id,
                                                    NodeHealtDefinitionKey::NODE_HEALT_LAST_ERROR_MESSAGE,
                                                    "",
                                                    false);
    HealtManager::getInstance()->addNodeMetricValue(control_unit_id,
                                                    NodeHealtDefinitionKey::NODE_HEALT_LAST_ERROR_DOMAIN,
                                                    "",
                                                    true);
    //restart the cotnrol unit
    CHAOS_NOT_THROW(start();)
    return true;
}

//! State machine is gone into recoverable error
void AbstractControlUnit::recoveredToState(int last_state) {
    ACULERR_ << "recoveredToState with state:" << last_state;
}

//! State machine is gone into an unrecoverable error
void AbstractControlUnit::fatalErrorFromState(int last_state, chaos::CException& ex) {
    ACULERR_ << "fatalErrorFromState with state:" << last_state;
    switch(last_state) {
        case CUStateKey::INIT:
            //deinit
            //CHAOS_NOT_THROW(redoInitSMCheckList(false);)
            deinit();
            CHAOS_NOT_THROW(redoInitRpCheckList(false);)
            break;
        case CUStateKey::DEINIT:
            
            break;
        case CUStateKey::START:
            //stop
            //CHAOS_NOT_THROW(redoStartSMCheckList(false);)
            stop();
            CHAOS_NOT_THROW(redoStartRpCheckList(false);)
            //deinit
            //CHAOS_NOT_THROW(redoInitSMCheckList(false);)
            deinit();
            CHAOS_NOT_THROW(redoInitRpCheckList(false);)
            break;
        case CUStateKey::STOP:
            break;
    }    //update healt tstatus to report recoverable error
    HealtManager::getInstance()->addNodeMetricValue(control_unit_id,
                                                    NodeHealtDefinitionKey::NODE_HEALT_STATUS,
                                                    NodeHealtDefinitionValue::NODE_HEALT_STATUS_FERROR,
                                                    false);
    HealtManager::getInstance()->addNodeMetricValue(control_unit_id,
                                                    NodeHealtDefinitionKey::NODE_HEALT_LAST_ERROR_CODE,
                                                    ex.errorCode,
                                                    false);
    HealtManager::getInstance()->addNodeMetricValue(control_unit_id,
                                                    NodeHealtDefinitionKey::NODE_HEALT_LAST_ERROR_MESSAGE,
                                                    ex.errorMessage,
                                                    false);
    HealtManager::getInstance()->addNodeMetricValue(control_unit_id,
                                                    NodeHealtDefinitionKey::NODE_HEALT_LAST_ERROR_DOMAIN,
                                                    ex.errorDomain,
                                                    true);
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
                                                              std::vector<std::string>& attribute_names) {
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
        attribute_setting.addAttribute(attribute_names[idx], attributeInfo.maxSize, attributeInfo.valueType, attributeInfo.binType);
        
        if(!attributeInfo.defaultValue.size()) continue;
        
        //setting value using index (the index into the sharedAttributeSetting are sequencial to the inserted order)
        try {
            switch (attributeInfo.valueType) {
                case DataType::TYPE_BOOLEAN : {
                    bool val = boost::lexical_cast<bool>(attributeInfo.defaultValue);
                    attribute_setting.setValueForAttribute(idx, &val, sizeof(bool));
                    break;
                }
                case DataType::TYPE_DOUBLE : {
                    double val = boost::lexical_cast<double>(attributeInfo.defaultValue);
                    attribute_setting.setValueForAttribute(idx, &val, sizeof(double));
                    break;
                }
                case DataType::TYPE_INT32 : {
                    int32_t val = strtoul(attributeInfo.defaultValue.c_str(),0,0);//boost::lexical_cast<int32_t>(attributeInfo.defaultValue);
                    attribute_setting.setValueForAttribute(idx, &val, sizeof(int32_t));
                    break;
                }
                case DataType::TYPE_INT64 : {
                    int64_t val = strtoll(attributeInfo.defaultValue.c_str(),0,0);//boost::lexical_cast<int64_t>(attributeInfo.defaultValue);
                    attribute_setting.setValueForAttribute(idx, &val, sizeof(int64_t));
                    break;
                }
                case DataType::TYPE_CLUSTER:{
                    CDataWrapper tmp;
                    tmp.setSerializedJsonData(attributeInfo.defaultValue.c_str());
                    attribute_setting.setValueForAttribute(idx, tmp);
                }
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
    
    domain_attribute_setting.addAttribute(DataPackCommonKey::DPCK_HIGH_RESOLUTION_TIMESTAMP, sizeof(uint64_t), DataType::TYPE_INT64);
    timestamp_hw_acq_cached_value = domain_attribute_setting.getValueSettingForIndex(domain_attribute_setting.getIndexForName(DataPackCommonKey::DPCK_HIGH_RESOLUTION_TIMESTAMP));
    
}

void AbstractControlUnit::completeInputAttribute() {
}

AbstractSharedDomainCache *AbstractControlUnit::_getAttributeCache() {
    return attribute_value_shared_cache;
}

void AbstractControlUnit::initSystemAttributeOnSharedAttributeCache() {
    AttributeCache& domain_attribute_setting = attribute_value_shared_cache->getSharedDomain(DOMAIN_SYSTEM);
    
    //add schedule time
    ACULDBG_ << "Adding system attribute on shared cache";
    domain_attribute_setting.addAttribute(ControlUnitDatapackSystemKey::THREAD_SCHEDULE_DELAY, 0, DataType::TYPE_INT64);
    thread_schedule_daly_cached_value = domain_attribute_setting.getValueSettingForIndex(domain_attribute_setting.getIndexForName(ControlUnitDatapackSystemKey::THREAD_SCHEDULE_DELAY));
    
    //add bypass state
    domain_attribute_setting.addAttribute(ControlUnitDatapackSystemKey::BYPASS_STATE, 0, DataType::TYPE_BOOLEAN);
    
    //add burst operation state
    domain_attribute_setting.addAttribute(ControlUnitDatapackSystemKey::BURST_STATE, 0, DataType::TYPE_BOOLEAN);
    
    //add burst operation tag
    domain_attribute_setting.addAttribute(ControlUnitDatapackSystemKey::BURST_TAG, 0, DataType::TYPE_STRING);
    
    //add storage type
    domain_attribute_setting.addAttribute(DataServiceNodeDefinitionKey::DS_STORAGE_TYPE, 0, DataType::TYPE_INT32);
    
    //add live time
    domain_attribute_setting.addAttribute(DataServiceNodeDefinitionKey::DS_STORAGE_LIVE_TIME, 0, DataType::TYPE_INT64);
    
    //add history time
    domain_attribute_setting.addAttribute(DataServiceNodeDefinitionKey::DS_STORAGE_HISTORY_TIME, 0, DataType::TYPE_INT64);
    
    //add history time
    domain_attribute_setting.addAttribute(DataServiceNodeDefinitionKey::DS_STORAGE_HISTORY_TIME, 0, DataType::TYPE_INT64);
    
    //add unit type
    domain_attribute_setting.addAttribute(DataPackSystemKey::DP_SYS_UNIT_TYPE, (uint32_t)control_unit_type.size(), DataType::TYPE_STRING);
    char * str_ptr = domain_attribute_setting.getValueSettingForIndex(domain_attribute_setting.getIndexForName(DataPackSystemKey::DP_SYS_UNIT_TYPE))->getValuePtr<char>();
    strncpy(str_ptr, control_unit_type.c_str(), control_unit_type.size());
}

/*
 Get the current control unit state
 */
CDWUniquePtr AbstractControlUnit::_getState(CDWUniquePtr getStatedParam){
    
    CreateNewDataWrapper(result,);
    result->addInt32Value(CUStateKey::CONTROL_UNIT_STATE, static_cast<CUStateKey::ControlUnitState>(SWEService::getServiceState()));
    return result;
}


CDWUniquePtr AbstractControlUnit::_submitStorageBurst(CDWUniquePtr data) {
    common::data::structured::DatasetBurstSDWrapper db_sdw;
    db_sdw.deserialize(data.get());
    DatasetBurstShrdPtr burst = ChaosMakeSharedPtr<DatasetBurst>(db_sdw());
    
    if(burst->type == chaos::ControlUnitNodeDefinitionType::DSStorageBurstTypeUndefined) {
        ACULERR_ << CHAOS_FORMAT("The type is mandatory for burst %1%", %data->getJSONString());
        return CDWUniquePtr();
    }
    if(!burst->value.isValid()) {
        ACULERR_ << CHAOS_FORMAT("The value is mandatory for burst %1%", %data->getJSONString());
        return CDWUniquePtr();
    }
    ACULDBG_<<"Enabling burst:"<<data->getCompliantJSONString();
    LQueueBurstWriteLock wl = burst_queue.getWriteLockObject();
    burst_queue().push(burst);
    return CDWUniquePtr();
}

CDWUniquePtr AbstractControlUnit::_datasetTagManagement(CDWUniquePtr data){
    CHECK_ASSERTION_THROW_AND_LOG(data!=NULL, ACULERR_, -1, "No parameter found");
    if(data->hasKey(ControlUnitNodeDomainAndActionRPC::ACTION_DATASET_TAG_MANAGEMENT_ADD_LIST)) {
        CHECK_KEY_THROW_AND_LOG(data,
                                ControlUnitNodeDomainAndActionRPC::ACTION_DATASET_TAG_MANAGEMENT_ADD_LIST,
                                ACULERR_,
                                -2,
                                CHAOS_FORMAT("The key %1% need to be a vector", %ControlUnitNodeDomainAndActionRPC::ACTION_DATASET_TAG_MANAGEMENT_ADD_LIST));
        ChaosStringSet ss;
        CMultiTypeDataArrayWrapperSPtr vec = data->getVectorValue(ControlUnitNodeDomainAndActionRPC::ACTION_DATASET_TAG_MANAGEMENT_REMOVE_LIST);
        for(int idx = 0; idx < vec->size(); idx++) {
            if(vec->isStringElementAtIndex(idx)) {
                ss.insert(vec->getStringElementAtIndex(idx));
            }
        }
        key_data_storage->addTag(ss);
    }
    
    if(data->hasKey(ControlUnitNodeDomainAndActionRPC::ACTION_DATASET_TAG_MANAGEMENT_REMOVE_LIST)) {
        CHECK_KEY_THROW_AND_LOG(data,
                                ControlUnitNodeDomainAndActionRPC::ACTION_DATASET_TAG_MANAGEMENT_REMOVE_LIST,
                                ACULERR_,
                                -3,
                                CHAOS_FORMAT("The key %1% need to be a vector", %ControlUnitNodeDomainAndActionRPC::ACTION_DATASET_TAG_MANAGEMENT_REMOVE_LIST));
        ChaosStringSet ss;
        CMultiTypeDataArrayWrapperSPtr vec = data->getVectorValue(ControlUnitNodeDomainAndActionRPC::ACTION_DATASET_TAG_MANAGEMENT_REMOVE_LIST);
        for(int idx = 0; idx < vec->size(); idx++) {
            if(vec->isStringElementAtIndex(idx)) {
                ss.insert(vec->getStringElementAtIndex(idx));
            }
        }
        key_data_storage->removeTag(ss);
    }
    
    return CDWUniquePtr();
}

/*
 Get the current control unit state
 */
CDWUniquePtr AbstractControlUnit::_getInfo(CDWUniquePtr getStatedParam) {
    CreateNewDataWrapper(stateResult,);
    //set the string representing the type of the control unit
    stateResult->addStringValue(NodeDefinitionKey::NODE_TYPE, NodeType::NODE_TYPE_CONTROL_UNIT);
    stateResult->addStringValue(NodeDefinitionKey::NODE_SUB_TYPE, control_unit_type);
    return stateResult;
}

void AbstractControlUnit::_updateAcquistionTimestamp(uint64_t alternative_ts) {
    *timestamp_acq_cached_value->getValuePtr<uint64_t>() = alternative_ts/1000;
    if(!use_custom_high_resolution_timestamp){
        *timestamp_hw_acq_cached_value->getValuePtr<uint64_t>() = alternative_ts;
    }
}

void AbstractControlUnit::useCustomHigResolutionTimestamp(bool _use_custom_high_resolution_timestamp) {
    use_custom_high_resolution_timestamp = _use_custom_high_resolution_timestamp;
}

void AbstractControlUnit::setHigResolutionAcquistionTimestamp(uint64_t high_resolution_timestamp) {
    if(use_custom_high_resolution_timestamp){
        *timestamp_hw_acq_cached_value->getValuePtr<uint64_t>() = high_resolution_timestamp;
    }
}

void AbstractControlUnit::_updateRunScheduleDelay(uint64_t new_scehdule_delay) {
    if(*thread_schedule_daly_cached_value->getValuePtr<uint64_t>() == new_scehdule_delay) return;
    //we need to update the value
    *thread_schedule_daly_cached_value->getValuePtr<uint64_t>() = new_scehdule_delay;
    thread_schedule_daly_cached_value->markAsChanged();
}

//!timer for update push metric
void AbstractControlUnit::_updatePushRateMetric() {
    uint64_t rate_acq_ts = TimingUtil::getTimeStamp();
    double time_offset = (double(rate_acq_ts - last_push_rate_grap_ts))/1000.0; //time in seconds
    double output_ds_rate = (time_offset>0)?push_dataset_counter/time_offset:0; //rate in seconds
    
    HealtManager::getInstance()->addNodeMetricValue(control_unit_id,
                                                    ControlUnitHealtDefinitionValue::CU_HEALT_OUTPUT_DATASET_PUSH_RATE,
                                                    output_ds_rate);
    
    //keep track of acquire timestamp
    last_push_rate_grap_ts = rate_acq_ts;
    //reset pushe count
    push_dataset_counter = 0;
}

//!put abstract control unit state machine in recoverable error
void AbstractControlUnit::_goInRecoverableError(chaos::CException recoverable_exception) {
    //change state machine
    if(SWEService::goInRecoverableError(this, recoverable_exception, "RTAbstractControlUnit", __PRETTY_FUNCTION__)) {
        //update healt the status to report recoverable error
        
    }
}

//!put abstract control unit state machine in fatal error
void AbstractControlUnit::_goInFatalError(chaos::CException recoverable_exception) {
    //change state machine
    if(SWEService::goInFatalError(this, recoverable_exception, "RTAbstractControlUnit", __PRETTY_FUNCTION__)) {
    }
}

void AbstractControlUnit::_completeDatasetAttribute() {
    
    //add busy flag
    DatasetDB::addAttributeToDataSet("busy",
                                     "Notify that the control unit is busy",
                                     DataType::TYPE_BOOLEAN,
                                     DataType::Output);
    
    //add global alarm checn
    DatasetDB::addAttributeToDataSet(stateVariableEnumToName(StateVariableTypeAlarmCU),
                                     "Activated when some warning has been issued",
                                     DataType::TYPE_INT32,
                                     DataType::Output);
    DatasetDB::addAttributeToDataSet(stateVariableEnumToName(StateVariableTypeAlarmDEV),
                                     "Activated when some alarm has been issued",
                                     DataType::TYPE_INT32,
                                     DataType::Output);
}

void AbstractControlUnit::_setBypassState(bool bypass_stage,
                                          bool high_priority) {
    DrvMsg cmd;
    memset(&cmd, 0, sizeof(DrvMsg));
    cmd.opcode = bypass_stage?OpcodeType::OP_SET_BYPASS:OpcodeType::OP_CLEAR_BYPASS;
    //broadcast bypass to all driver instances allocated by control unit
    for(VInstantitedDriverIterator it = accessor_instances.begin(),
        end = accessor_instances.end();
        it != end;
        it++) {
        (*it)->send(&cmd, (high_priority?1000:0));
    }
    //update dateset
    *attribute_value_shared_cache->getAttributeValue(DOMAIN_SYSTEM, ControlUnitDatapackSystemKey::BYPASS_STATE)->getValuePtr<bool>() = bypass_stage;
}

//!handler calledfor restor a control unit to a determinate point
bool AbstractControlUnit::unitRestoreToSnapshot(AbstractSharedDomainCache * const snapshot_cache) throw(CException) {
    return true;
}

//! this andler is called befor the input attribute will be updated
void AbstractControlUnit::unitInputAttributePreChangeHandler() throw(CException) {}

//! attribute change handler
/*!
 the handle is fired after the input attribute cache as been update triggere
 by the rpc request for attribute change.
 */
void AbstractControlUnit::unitInputAttributeChangedHandler() throw(CException) {}


#define CHECK_FOR_RANGE_VALUE(t, v, attr_name)\
t max,min;\
if(attributeInfo.maxRange.compare(0,2,"0x")==0){max = strtoll(attributeInfo.maxRange.c_str(),0,0);}else{max = attributeInfo.maxRange.size()?boost::lexical_cast<t>(attributeInfo.maxRange):std::numeric_limits<t>::max();}\
if(attributeInfo.minRange.compare(0,2,"0x")==0){min = strtoll(attributeInfo.minRange.c_str(),0,0);}else{min = attributeInfo.minRange.size()?boost::lexical_cast<t>(attributeInfo.minRange):std::numeric_limits<t>::min();}\
if(v < min || v > max) throw MetadataLoggingCException(getCUID(), -1,  boost::str(boost::format("Invalid value (%1%) [Min:%2% Max:%3%] for attribute %4%") % v % attributeInfo.minRange % attributeInfo.maxRange % attr_name ).c_str(), __PRETTY_FUNCTION__);\

#define CHECK_FOR_STRING_RANGE_VALUE(v, attr_name)\
if(attributeInfo.minRange.size() && v < attributeInfo.minRange ) throw MetadataLoggingCException(getCUID(), -1, boost::str(boost::format("Invalid value (%1%) [max:%2% Min:%3%] for attribute %4%") % v % attr_name % attributeInfo.minRange % attributeInfo.maxRange).c_str(), __PRETTY_FUNCTION__);\
if(attributeInfo.maxRange.size() && v > attributeInfo.maxRange ) throw MetadataLoggingCException(getCUID(), -1, boost::str(boost::format("Invalid value (%1%) [max:%2% Min:%3%] for attribute %4%") % v % attr_name %attributeInfo.minRange % attributeInfo.maxRange).c_str(), __PRETTY_FUNCTION__);\

CDWUniquePtr AbstractControlUnit::setDatasetAttribute(CDWUniquePtr dataset_attribute_values) {
    CHAOS_ASSERT(dataset_attribute_values.get())
    ChaosSharedPtr<SharedCacheLockDomain> w_lock = attribute_value_shared_cache->getLockOnDomain(DOMAIN_INPUT, true);
    w_lock->lock();
    
    RangeValueInfo attributeInfo;
    std::vector<std::string> in_attribute_name;
    
    try {
        //call pre handler
        unitInputAttributePreChangeHandler();
        
        //first call attribute handler
        dataset_attribute_manager.executeHandlers(dataset_attribute_values.get());
        
        //get all input attribute name for input and bidirectional directions
        getDatasetAttributesName(DataType::Input , in_attribute_name);
        getDatasetAttributesName(DataType::Bidirectional , in_attribute_name);
        
        if(dataset_attribute_values->hasKey(NodeDefinitionKey::NODE_UNIQUE_ID)) {
            //get the contrl unit id
            std::string node_id = dataset_attribute_values->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
            //compare the message device id and the local
            for (std::vector<std::string>::iterator iter = in_attribute_name.begin();
                 iter != in_attribute_name.end();
                 iter++) {
                //execute attribute handler
                const char * attr_name = iter->c_str();
                
                //check if the attribute name is present
                if(dataset_attribute_values->hasKey(attr_name)) {
                    
                    //check if attribute has been accepted
                    if(dataset_attribute_manager.getHandlerResult(*iter) == false) continue;
                    
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
                            
                        case DataType::TYPE_CLUSTER: {
                            ChaosUniquePtr<CDataWrapper> str = dataset_attribute_values->getCSDataValue(attr_name);
                            try{
                                if(str.get()){
                                    attribute_cache_value->setValue(*(str.get()));
                                }
                            } catch(...){
                                throw MetadataLoggingCException(getCUID(), -1, boost::str(boost::format("Invalid Json format ")  ).c_str(),__PRETTY_FUNCTION__);
                            }
                            break;
                        }
                        case DataType::TYPE_STRING: {
                            std::string str = dataset_attribute_values->getStringValue(attr_name);
                            CHECK_FOR_STRING_RANGE_VALUE(str, attr_name)
                            attribute_cache_value->setValue(str.c_str(), (uint32_t)str.size());
                            break;
                        }
                        case DataType::TYPE_BYTEARRAY: {
                            uint32_t bin_size = 0;
                            const char *binv = dataset_attribute_values->getBinaryValue(attr_name, bin_size);
                            attribute_cache_value->setValue(binv, bin_size);
                            break;
                        }
                        default:
                            break;
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
        throw;
    }
    return CDWUniquePtr();
}

/*
 Update the configuration for all descendant tree in the Control Unit class structure
 */
CDWUniquePtr AbstractControlUnit::updateConfiguration(CDWUniquePtr update_pack) {
    //check to see if the device can ben initialized
    if(SWEService::getServiceState() != chaos::CUStateKey::INIT &&
       SWEService::getServiceState() != chaos::CUStateKey::START) {
        ACULAPP_ << "device:" << DatasetDB::getDeviceID() << " not initialized";
        throw MetadataLoggingCException(getCUID(), -3, "Device Not Initilized", __PRETTY_FUNCTION__);
    }
    
    PropertyGroupVectorSDWrapper pg_sdw;
    pg_sdw.serialization_key="property";
    pg_sdw.deserialize(update_pack.get());
    
    //update the property
    PropertyCollector::applyValue(pg_sdw());
    key_data_storage->updateConfiguration(update_pack.get());
    //mark all cache as changed
    attribute_value_shared_cache->getSharedDomain(DOMAIN_SYSTEM).markAllAsChanged();
    
    pushSystemDataset();
    return CDWUniquePtr();
}

bool AbstractControlUnit::propertyChangeHandler(const std::string& group_name,
                                                const std::string& property_name,
                                                const CDataVariant& property_value) {
    ACULDBG_ << CHAOS_FORMAT("Update property request for %1%[%2%] with value %3%", %property_name%group_name%property_value.asString());
    return true;
}

void AbstractControlUnit::propertyUpdatedHandler(const std::string& group_name,
                                                 const std::string& property_name,
                                                 const CDataVariant& old_value,
                                                 const CDataVariant& new_value) {
    if(group_name.compare("property_abstract_control_unit") == 0) {
        //update property on driver
        key_data_storage->updateConfiguration(property_name, new_value);
        //reflect modification on dataset
        if(property_name.compare(ControlUnitDatapackSystemKey::BYPASS_STATE) == 0) {
            _setBypassState(new_value.asBool());
        } else if(property_name.compare(DataServiceNodeDefinitionKey::DS_STORAGE_TYPE) == 0) {
            *attribute_value_shared_cache->getAttributeValue(DOMAIN_SYSTEM, DataServiceNodeDefinitionKey::DS_STORAGE_TYPE)->getValuePtr<int32_t>() = new_value.asInt32();
        } else if(property_name.compare(DataServiceNodeDefinitionKey::DS_STORAGE_LIVE_TIME) == 0) {
            *attribute_value_shared_cache->getAttributeValue(DOMAIN_SYSTEM, DataServiceNodeDefinitionKey::DS_STORAGE_LIVE_TIME)->getValuePtr<uint64_t>() = new_value.asUInt64();
        } else if(property_name.compare(DataServiceNodeDefinitionKey::DS_STORAGE_HISTORY_TIME) == 0) {
            *attribute_value_shared_cache->getAttributeValue(DOMAIN_SYSTEM, DataServiceNodeDefinitionKey::DS_STORAGE_HISTORY_TIME)->getValuePtr<uint64_t>() = new_value.asUInt64();
        }
    }
}

//! return the accessor by an index
/*
 The index parameter correspond to the order that the driver infromation are
 added by the unit implementation into the function AbstractControlUnit::unitDefineDriver.
 */
DriverAccessor *AbstractControlUnit::getAccessoInstanceByIndex(int idx) {
    if( idx >= accessor_instances.size() ) return NULL;
    return accessor_instances[idx];
}


int AbstractControlUnit::pushOutputDataset() {
    int err=0;
    AttributeCache& output_attribute_cache = attribute_value_shared_cache->getSharedDomain(DOMAIN_OUTPUT);
    ChaosSharedPtr<SharedCacheLockDomain> r_lock = attribute_value_shared_cache->getLockOnDomain(DOMAIN_OUTPUT, false);
    r_lock->lock();
    
    //check if something as changed
    if(!output_attribute_cache.hasChanged()) return err;
    
    CDWShrdPtr output_attribute_dataset = key_data_storage->getNewDataPackForDomain(KeyDataStorageDomainOutput);
    if(!output_attribute_dataset) return err;
    output_attribute_dataset->addInt64Value(ControlUnitDatapackCommonKey::RUN_ID, run_id);
    output_attribute_dataset->addInt64Value(DataPackCommonKey::DPCK_TIMESTAMP, *timestamp_acq_cached_value->getValuePtr<uint64_t>());
    output_attribute_dataset->addInt64Value(DataPackCommonKey::DPCK_HIGH_RESOLUTION_TIMESTAMP, *timestamp_hw_acq_cached_value->getValuePtr<uint64_t>());
    
    //add all other output channel
    for(int idx = 0;
        idx < ((int)cache_output_attribute_vector.size())-1; //the device id and timestamp in added out of this list
        idx++) {
        //
        AttributeValue * value_set = cache_output_attribute_vector[idx];
        assert(value_set);
        
        switch(value_set->type) {
            case DataType::TYPE_BOOLEAN:
                output_attribute_dataset->addBoolValue(value_set->name, *value_set->getValuePtr<bool>());
                break;
            case DataType::TYPE_INT32:
                output_attribute_dataset->addInt32Value(value_set->name, *value_set->getValuePtr<int32_t>());
                break;
            case DataType::TYPE_INT64:
                output_attribute_dataset->addInt64Value(value_set->name, *value_set->getValuePtr<int64_t>());
                break;
            case DataType::TYPE_DOUBLE:
                output_attribute_dataset->addDoubleValue(value_set->name, *value_set->getValuePtr<double>());
                break;
            case DataType::TYPE_CLUSTER:{
                
                try{
                    output_attribute_dataset->addCSDataValue(value_set->name,*value_set->getValuePtr<CDataWrapper>());
                } catch(...){
                    throw MetadataLoggingCException(getCUID(), -101, boost::str(boost::format("Invalid Json format for attribute '%1%' :'%2%")  % value_set->name %value_set->getValuePtr<const char>()).c_str(),__PRETTY_FUNCTION__);
                    
                }
                break;
            }
            case DataType::TYPE_STRING:
                //DEBUG_CODE(ACULAPP_ << value_set->name<<"-"<<value_set->getValuePtr<const char>();)
                output_attribute_dataset->addStringValue(value_set->name, value_set->getValuePtr<const char>());
                break;
            case DataType::TYPE_BYTEARRAY:
                if(value_set->size) {
                    if(value_set->sub_type.size() == 1) {
                        output_attribute_dataset->addBinaryValue(value_set->name, value_set->sub_type[0],value_set->getValuePtr<char>(), value_set->size);
                    } else {
                        output_attribute_dataset->addBinaryValue(value_set->name,value_set->getValuePtr<char>(), value_set->size);
                    }
                }
                break;
            default:
                break;
        }
    }
    //manage the burst information
    manageBurstQueue();
    //now we nede to push the outputdataset
    err=key_data_storage->pushDataSet(data_manager::KeyDataStorageDomainOutput, MOVE(output_attribute_dataset));
    
    //update counter
    push_dataset_counter++;
    
    //reset chagned attribute into output dataset
    output_attribute_cache.resetChangedIndex();
    return err;
}

//push system dataset
int AbstractControlUnit::pushInputDataset() {
    int err=0;
    AttributeCache& input_attribute_cache = attribute_value_shared_cache->getSharedDomain(DOMAIN_INPUT);
    if(!input_attribute_cache.hasChanged()) return err;
    //get the cdatawrapper for the pack
    int64_t cur_us = TimingUtil::getTimeStampInMicroseconds();
    CDWShrdPtr input_attribute_dataset = key_data_storage->getNewDataPackForDomain(KeyDataStorageDomainInput);
    if(input_attribute_dataset) {
        input_attribute_dataset->addInt64Value(ControlUnitDatapackCommonKey::RUN_ID, run_id);
        //input dataset timestamp is added only when pushed on cache
        input_attribute_dataset->addInt64Value(DataPackCommonKey::DPCK_TIMESTAMP, cur_us/1000);
        input_attribute_dataset->addInt64Value(DataPackCommonKey::DPCK_HIGH_RESOLUTION_TIMESTAMP, cur_us);
        //fill the dataset
        fillCDatawrapperWithCachedValue(cache_input_attribute_vector, *input_attribute_dataset);
        
        //push out the system dataset
        err=key_data_storage->pushDataSet(data_manager::KeyDataStorageDomainInput, MOVE(input_attribute_dataset));
    }
    input_attribute_cache.resetChangedIndex();
    return err;
}

//push system dataset
int AbstractControlUnit::pushCustomDataset() {
    int err=0;
    AttributeCache& custom_attribute_cache = attribute_value_shared_cache->getSharedDomain(DOMAIN_CUSTOM);
    if(!custom_attribute_cache.hasChanged()) return err;
    //get the cdatawrapper for the pack
    int64_t cur_us = TimingUtil::getTimeStampInMicroseconds();
    CDWShrdPtr custom_attribute_dataset = key_data_storage->getNewDataPackForDomain(KeyDataStorageDomainCustom);
    if(custom_attribute_dataset) {
        custom_attribute_dataset->addInt64Value(ControlUnitDatapackCommonKey::RUN_ID, run_id);
        //input dataset timestamp is added only when pushed on cache
        custom_attribute_dataset->addInt64Value(DataPackCommonKey::DPCK_TIMESTAMP, cur_us/1000);
        custom_attribute_dataset->addInt64Value(DataPackCommonKey::DPCK_HIGH_RESOLUTION_TIMESTAMP, cur_us);
        
        //fill the dataset
        fillCDatawrapperWithCachedValue(cache_custom_attribute_vector, *custom_attribute_dataset);
        
        //push out the system dataset
        err=key_data_storage->pushDataSet(data_manager::KeyDataStorageDomainCustom, MOVE(custom_attribute_dataset));
    }
    return err;
}

int AbstractControlUnit::pushSystemDataset() {
    int err=0;
    AttributeCache& system_attribute_cache = attribute_value_shared_cache->getSharedDomain(DOMAIN_SYSTEM);
    if(!system_attribute_cache.hasChanged()) return err;
    //get the cdatawrapper for the pack
    int64_t cur_us = TimingUtil::getTimeStampInMicroseconds();
    CDWShrdPtr system_attribute_dataset = key_data_storage->getNewDataPackForDomain(KeyDataStorageDomainSystem);
    if(system_attribute_dataset) {
        system_attribute_dataset->addInt64Value(ControlUnitDatapackCommonKey::RUN_ID, run_id);
        //input dataset timestamp is added only when pushed on cache
        system_attribute_dataset->addInt64Value(DataPackCommonKey::DPCK_TIMESTAMP, cur_us/1000);
        system_attribute_dataset->addInt64Value(DataPackCommonKey::DPCK_HIGH_RESOLUTION_TIMESTAMP, cur_us);
        //fill the dataset
        fillCDatawrapperWithCachedValue(cache_system_attribute_vector, *system_attribute_dataset);
        //push out the system dataset
        err=key_data_storage->pushDataSet(data_manager::KeyDataStorageDomainSystem, MOVE(system_attribute_dataset));
    }
    //reset changed index
    system_attribute_cache.resetChangedIndex();
    return err;
}

CDWShrdPtr AbstractControlUnit::writeCatalogOnCDataWrapper(AlarmCatalog& catalog,
                                                           int32_t dataset_type) {
    CDWShrdPtr attribute_dataset = key_data_storage->getNewDataPackForDomain((KeyDataStorageDomain)dataset_type);
    
    if(attribute_dataset) {
        //fill datapack with
        //! the dataaset can be pushed also in other moment
        attribute_dataset->addInt64Value(DataPackCommonKey::DPCK_TIMESTAMP, TimingUtil::getTimeStamp());
        //scan all alarm ad create the datapack
        size_t alarm_size = catalog.size();
        for(unsigned int idx = 0;
            idx < alarm_size;
            idx++) {
            AlarmDescription *alarm = catalog.getAlarmByOrderedID(idx);
            attribute_dataset->addInt32Value(alarm->getAlarmName(),
                                             (uint32_t)alarm->getCurrentSeverityCode());
        }
    }
    return attribute_dataset;
}

int AbstractControlUnit::pushDevAlarmDataset() {
    GET_CAT_OR_EXIT(StateVariableTypeAlarmDEV,0 );
    int err=0;

    CDWShrdPtr attribute_dataset = writeCatalogOnCDataWrapper(catalog,
                                                              DataPackCommonKey::DPCK_DATASET_TYPE_DEV_ALARM);
    if(attribute_dataset) {
        //push out the system dataset
        err=key_data_storage->pushDataSet(KeyDataStorageDomainDevAlarm, MOVE(attribute_dataset));
    }
    return err;
}

int AbstractControlUnit::pushCUAlarmDataset() {
    GET_CAT_OR_EXIT(StateVariableTypeAlarmCU,0 );
    int err=0;
    CDWShrdPtr attribute_dataset = writeCatalogOnCDataWrapper(catalog,
                                                              DataPackCommonKey::DPCK_DATASET_TYPE_CU_ALARM);
    if(attribute_dataset) {
        //push out the system dataset
        err=key_data_storage->pushDataSet(KeyDataStorageDomainCUAlarm, MOVE(attribute_dataset));
    }
    return err;
}

void AbstractControlUnit::manageBurstQueue() {
    if(!current_burst.get()) {
        LQueueBurstReadLock wl = burst_queue.getReadLockObject();
        if(!burst_queue().empty()){
            switch(burst_queue().front()->type) {
                case chaos::ControlUnitNodeDefinitionType::DSStorageBurstTypeNPush:
                    current_burst.reset(new PushStorageBurst(MOVE(burst_queue().front())));
                    break;
                case chaos::ControlUnitNodeDefinitionType::DSStorageBurstTypeMSec:
                    current_burst.reset(new MSecStorageBurst(MOVE(burst_queue().front())));
                    break;
                default:
                    break;
            }
            burst_queue().pop();
            //set the tag for burst
            ACULDBG_<<"===Start Burst tag:'"<<current_burst->dataset_burst->tag<<"' ====";
            key_data_storage->addTag(current_burst->dataset_burst->tag);
            key_data_storage->setTimingConfigurationBehaviour(false);
            key_data_storage->setOverrideStorageType(DataServiceNodeDefinitionType::DSStorageTypeHistory);
            *attribute_value_shared_cache->getAttributeValue(DOMAIN_SYSTEM, ControlUnitDatapackSystemKey::BURST_STATE)->getValuePtr<bool>() = true;
            attribute_value_shared_cache->getAttributeValue(DOMAIN_SYSTEM, ControlUnitDatapackSystemKey::BURST_TAG)->setStringValue(current_burst->dataset_burst->tag, true, true);
            attribute_value_shared_cache->getSharedDomain(DOMAIN_SYSTEM).markAllAsChanged();
            pushSystemDataset();
        }
    } else {
        
        if(!current_burst->active(timestamp_acq_cached_value->getValuePtr<int64_t>())) {
            //remove the tag for the burst
            ACULDBG_<<"=== End Burst tag:'"<<current_burst->dataset_burst->tag<<"'  =======";

            key_data_storage->removeTag(current_burst->dataset_burst->tag);
            key_data_storage->setTimingConfigurationBehaviour(true);
            key_data_storage->setOverrideStorageType(DataServiceNodeDefinitionType::DSStorageTypeUndefined);
            *attribute_value_shared_cache->getAttributeValue(DOMAIN_SYSTEM, ControlUnitDatapackSystemKey::BURST_STATE)->getValuePtr<bool>() = false;
            attribute_value_shared_cache->getAttributeValue(DOMAIN_SYSTEM, ControlUnitDatapackSystemKey::BURST_TAG)->setStringValue("");
            attribute_value_shared_cache->getSharedDomain(DOMAIN_SYSTEM).markAllAsChanged();
            //we need to remove it
            current_burst.reset();
            pushSystemDataset();
        }
    }
}

void AbstractControlUnit::fillCDatawrapperWithCachedValue(std::vector<AttributeValue*>& cached_attributes, CDataWrapper& dataset) {
    for(std::vector<AttributeValue*>::iterator it = cached_attributes.begin();
        it != cached_attributes.end();
        it++) {
        (*it)->writeToCDataWrapper(dataset);
    }
}

//!timer for update push metric
void AbstractControlUnit::timeout() {
    //update push metric
    _updatePushRateMetric();
}

bool AbstractControlUnit::isInputAttributeChangeAuthorizedByHandler(const std::string& attr_name) {
    return dataset_attribute_manager.getHandlerResult(attr_name);
}

void AbstractControlUnit::copyInitConfiguraiton(CDataWrapper& copy) {
    if(init_configuration.get() == NULL) return;
    
    //copy all key
    init_configuration->copyAllTo(copy);
}

#pragma mark Abstract Control Unit API
void AbstractControlUnit::addStateVariable(StateVariableType variable_type,
                                           const std::string& state_variable_name,
                                           const std::string& state_variable_description) {
    if(map_variable_catalog.count(variable_type) == 0) {
        //add new catalog
        map_variable_catalog.insert(MapStateVariablePair(variable_type, AlarmCatalog(stateVariableEnumToName(variable_type))));
    }
    AlarmCatalog& catalog = map_variable_catalog[variable_type];
    catalog.addAlarm(new MultiSeverityAlarm(stateVariableEnumToName(variable_type),
                                            state_variable_name,
                                            state_variable_description));
    //add this instance as
    catalog.addAlarmHandler(state_variable_name,
                            this);
}

void AbstractControlUnit::setStateVariableSeverity(StateVariableType variable_type,
                                                   const common::alarm::MultiSeverityAlarmLevel state_variable_severity) {
    GET_CAT_OR_EXIT(variable_type, )
    catalog.setAllAlarmSeverity(state_variable_severity);
    AttributeCache& output_cache = attribute_value_shared_cache->getSharedDomain(DOMAIN_OUTPUT);
    output_cache.getValueSettingByName(stateVariableEnumToName(variable_type))->setValue(CDataVariant(catalog.max()));
}

bool AbstractControlUnit::setStateVariableSeverity(StateVariableType variable_type,
                                                   const std::string& state_variable_name,
                                                   const MultiSeverityAlarmLevel state_variable_severity) {
    GET_CAT_OR_EXIT(variable_type, false)
    AlarmDescription *alarm = catalog.getAlarmByName(state_variable_name);
    if(alarm == NULL) return false;
    alarm->setCurrentSeverity(state_variable_severity);
    //update global alarm output attribute
    AttributeCache& output_cache = attribute_value_shared_cache->getSharedDomain(DOMAIN_OUTPUT);
    output_cache.getValueSettingByName(stateVariableEnumToName(variable_type))->setValue(CDataVariant(catalog.max()));
    return true;
}

bool AbstractControlUnit::setStateVariableSeverity(StateVariableType variable_type,
                                                   const unsigned int state_variable_ordered_id,
                                                   const MultiSeverityAlarmLevel state_variable_severity) {
    GET_CAT_OR_EXIT(variable_type, false)
    AlarmDescription *alarm = catalog.getAlarmByOrderedID(state_variable_ordered_id);
    if(alarm == NULL) return false;
    alarm->setCurrentSeverity(state_variable_severity);
    //update global alarm output attribute
    AttributeCache& output_cache = attribute_value_shared_cache->getSharedDomain(DOMAIN_OUTPUT);
    output_cache.getValueSettingByName(stateVariableEnumToName(variable_type))->setValue(CDataVariant(catalog.isCatalogClear()==false));
    return true;
}

bool AbstractControlUnit::getStateVariableSeverity(StateVariableType variable_type,
                                                   const std::string& state_variable_name,
                                                   MultiSeverityAlarmLevel& state_variable_severity) {
    GET_CAT_OR_EXIT(variable_type, false)
    AlarmDescription *alarm = catalog.getAlarmByName(state_variable_name);
    if(alarm == NULL) return false;
    state_variable_severity = static_cast<MultiSeverityAlarmLevel>(alarm->getCurrentSeverityCode());
    return true;
}

bool AbstractControlUnit::getStateVariableSeverity(StateVariableType variable_type,
                                                   const unsigned int state_variable_ordered_id,
                                                   MultiSeverityAlarmLevel& state_variable_severity) {
    GET_CAT_OR_EXIT(variable_type, false)
    AlarmDescription *alarm = catalog.getAlarmByOrderedID(state_variable_ordered_id);
    if(alarm == NULL) return false;
    state_variable_severity = static_cast<MultiSeverityAlarmLevel>(alarm->getCurrentSeverityCode());
    return true;
}

void AbstractControlUnit::alarmChanged(const std::string& state_variable_tag,
                                       const std::string& state_variable_name,
                                       const int8_t state_variable_severity) {
    int variable_type = stateVariableNameToEnum(state_variable_tag);
    if(variable_type == -1) return;
    
    GET_CAT_OR_EXIT((StateVariableType)variable_type, )
    AlarmDescription *alarm = catalog.getAlarmByName(state_variable_name);
    CHAOS_ASSERT(alarm);
    
    //update alarm log
    alarm_logging_channel->logAlarm(getCUID(),
                                    "AbstractControlUnit",
                                    *alarm);
    
    switch((StateVariableType)variable_type) {
        case StateVariableTypeAlarmCU:
            //update dataset alarm on cds
            pushCUAlarmDataset();
            break;
            
        case StateVariableTypeAlarmDEV:
            //update dataset alarm on cds
            pushDevAlarmDataset();
            break;
    }
}

void AbstractControlUnit::setBusyFlag(bool state) {
    AttributeCache& output_cache = attribute_value_shared_cache->getSharedDomain(DOMAIN_OUTPUT);
    if(output_cache.hasName("busy")) {
        output_cache.getValueSettingByName("busy")->setValue(CDataVariant(state));
    }
}

const bool AbstractControlUnit::getBusyFlag() const {
    AttributeCache& output_cache = attribute_value_shared_cache->getSharedDomain(DOMAIN_OUTPUT);
    if(output_cache.hasName("busy")) {
        return output_cache.getValueSettingByName("busy")->getAsVariant().asBool();
    }else {
        return false;
    }
}

void AbstractControlUnit::metadataLogging(const std::string& subject,
                                          const chaos::common::metadata_logging::StandardLoggingChannel::LogLevel log_level,
                                          const std::string& message) {
    if(standard_logging_channel == NULL) return;
    
    standard_logging_channel->logMessage(getCUID(),
                                         subject,
                                         log_level,
                                         message);
    switch (log_level) {
        case StandardLoggingChannel::LogLevelInfo:
            ACULAPP_ << log_level;
            break;
        case StandardLoggingChannel::LogLevelDebug:
            ACULDBG_ << log_level;
            break;
        case StandardLoggingChannel::LogLevelWarning:
            ACULWRN_ << log_level;
            break;
        case StandardLoggingChannel::LogLevelError:
            ACULERR_ << log_level;
            break;
        case StandardLoggingChannel::LogLevelFatal:
            ACULNOTE_ << log_level;
            break;
    }
}

void AbstractControlUnit::metadataLogging(const StandardLoggingChannel::LogLevel log_level,
                                          const std::string& message) {
    metadataLogging("AbstractControlUnit",
                    log_level,
                    message);
}
