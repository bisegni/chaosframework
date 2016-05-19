/*
 *	AbstractExecutionUnit.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 26/04/16 INFN, National Institute of Nuclear Physics
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

#include <chaos/common/utility/endianess.h>
#include <chaos/common/io/IODirectIODriver.h>
#include <chaos/common/network/NetworkBroker.h>

#include <chaos/cu_toolkit/control_manager/AbstractExecutionUnit.h>

#include <boost/format.hpp>

using namespace chaos;
using namespace chaos::common::io;
using namespace chaos::common::data;
using namespace chaos::common::network;
using namespace chaos::common::utility;
using namespace chaos::common::message;
using namespace chaos::common::exception;
using namespace chaos::common::data::cache;

using namespace chaos::cu;
using namespace chaos::cu::control_manager;

using namespace boost;
using namespace boost::chrono;

#define EULAPP_ INFO_LOG_1_P(AbstractExecutionUnit, getCUInstance())
#define EULDBG_ DBG_LOG_1_P(AbstractExecutionUnit, getCUInstance())
#define EULERR_ ERR_LOG_1_P(AbstractExecutionUnit, getCUInstance())

AbstractExecutionUnit::AbstractExecutionUnit(const std::string& _execution_unit_id,
                                             const std::string& _execution_unit_param):
RTAbstractControlUnit(CUType::EXUT,
                      _execution_unit_id,
                      _execution_unit_param),
mds_msg_chnl(NULL),
data_driver(),
last_execution_ts(0){}

/*!
 Parametrized constructor
 \param _execution_unit_id unique id for the control unit
 \param _execution_unit_drivers driver information
 */
AbstractExecutionUnit::AbstractExecutionUnit(const std::string& _execution_unit_id,
                                             const std::string& _execution_unit_param,
                                             const ControlUnitDriverList& _execution_unit_drivers):
RTAbstractControlUnit(CUType::EXUT,
                      _execution_unit_id,
                      _execution_unit_param,
                      _execution_unit_drivers),
mds_msg_chnl(NULL),
data_driver(),
last_execution_ts(0){}

AbstractExecutionUnit::~AbstractExecutionUnit() {}

#pragma Execution Uni tAPI

void AbstractExecutionUnit::setExecutionUnitDescription(const std::string& description) {
    eu_description = description;
}


void AbstractExecutionUnit::addExecutionUnitVariable(const std::string& alias,
                                                     const std::string& description,
                                                     chaos::DataType::DataType type,
                                                     DataType::DataSetAttributeIOAttribute direction,
                                                     bool mandatory) {
    if(variable_map.count(alias) != 0) {throw MetadataLoggingCException(getCUID(), -10000, "Variable alread added", __PRETTY_FUNCTION__);}
        //add the variable to the list
    variable_map.insert(make_pair(alias, VariableParameterDefinition(alias,
                                                                     description,
                                                                     type,
                                                                     direction,
                                                                     mandatory)));
}

void AbstractExecutionUnit::setOutputVariableValue(const std::string& variable_name,
                                                   const void * value_ptr,
                                                   uint32_t value_size) {
    output_variable_cache->setValueForAttribute(variable_name,
                                                value_ptr,
                                                value_size);
}


void AbstractExecutionUnit::setOutputVariableValue(const std::string& variable_name,
                                                   bool value) {
    bool tmp_val = value;
    output_variable_cache->setValueForAttribute(variable_name,
                                                &tmp_val,
                                                sizeof(bool));
}


void AbstractExecutionUnit::setOutputVariableValue(const std::string& variable_name,
                                                   uint32_t value) {
    uint32_t tmp_val = value;
    output_variable_cache->setValueForAttribute(variable_name,
                                                &tmp_val,
                                                sizeof(uint32_t));
}

void AbstractExecutionUnit::setOutputVariableValue(const std::string& variable_name,
                                                   uint64_t value) {
    uint64_t tmp_val = value;
    output_variable_cache->setValueForAttribute(variable_name,
                                                &tmp_val,
                                                sizeof(uint64_t));
}

void AbstractExecutionUnit::setOutputVariableValue(const std::string& variable_name,
                                                   double value) {
    double tmp_val = value;
    output_variable_cache->setValueForAttribute(variable_name,
                                                &tmp_val,
                                                sizeof(double));
}

void AbstractExecutionUnit::setOutputVariableValue(const std::string& variable_name,
                                                   const std::string& value) {
    output_variable_cache->setNewSize(variable_name,
                                      (uint32_t)value.size(),
                                      true);
    output_variable_cache->setValueForAttribute(variable_name,
                                                value.c_str(),
                                                (uint32_t)value.size());
}

#pragma internal method
#define CHECK_PRESENCE_AND_TYPE(o, t, k)\
(o->hasKey(k) && o->t(k))

void AbstractExecutionUnit::completeVaribleAndAllocateDMC(CDataWrapper& variable_configuration) throw(CException) {
        //map_nuid_dmc
    if(variable_configuration.hasKey(ExecutionUnitNodeDefinitionKey::EXECUTION_UNIT_VARIABLE_LIST) &&
       variable_configuration.isVector(ExecutionUnitNodeDefinitionKey::EXECUTION_UNIT_VARIABLE_LIST)) {
        EULAPP_ << "No variable list foudn into initialization package";
        return;
    }

    try {
            //! scann al variable
        std::auto_ptr<CMultiTypeDataArrayWrapper> var_vec(variable_configuration.getVectorValue(ExecutionUnitNodeDefinitionKey::EXECUTION_UNIT_VARIABLE_LIST));
        for(unsigned int idx = 0;
            idx < var_vec->size();
            idx++) {
            std::auto_ptr<CDataWrapper> variable_desc(var_vec->getCDataWrapperElementAtIndex(idx));
            if(CHECK_PRESENCE_AND_TYPE(variable_desc, isStringValue, ExecutionUnitNodeDefinitionKey::EXECUTION_UNIT_VAR_ALIAS) &&
               CHECK_PRESENCE_AND_TYPE(variable_desc, isStringValue, ExecutionUnitNodeDefinitionKey::EXECUTION_UNIT_VAR_NODE_UID) &&
               CHECK_PRESENCE_AND_TYPE(variable_desc, isStringValue, ExecutionUnitNodeDefinitionKey::EXECUTION_UNIT_VAR_DATASET_ATTRIBUTE) ) {

                VPDMapIterator found_variable = variable_map.find(variable_desc->getStringValue(ExecutionUnitNodeDefinitionKey::EXECUTION_UNIT_VAR_ALIAS));
                if(found_variable == variable_map.end()) continue;

                found_variable->second.node_uid = variable_desc->getStringValue(ExecutionUnitNodeDefinitionKey::EXECUTION_UNIT_VAR_NODE_UID);
                found_variable->second.node_dataset_attribute = variable_desc->getStringValue(ExecutionUnitNodeDefinitionKey::EXECUTION_UNIT_VAR_DATASET_ATTRIBUTE);

                    //add variableto cache
                addVariableInCache(found_variable->second);

                    //we need only device message channel for variable bidirectinal and output direction type
                if(found_variable->second.direction == DataType::Input) continue;

                    //we have foudn the variable
                CDeviceNetworkAddress *node_address;
                if(mds_msg_chnl->getNetworkAddressForDevice(found_variable->second.node_uid,
                                                            &node_address)) {
                    EULERR_ << "Error fetching address for node:" << found_variable->second.node_uid;
                    continue;
                }
                    //get message channel for device uid
                DeviceMessageChannel *tmp_msg_chnl = NetworkBroker::getInstance()->getDeviceMessageChannelFromAddress(node_address);
                if(tmp_msg_chnl == NULL){
                    EULERR_ << "Error fetching device message channel for:" << found_variable->second.node_uid;
                    continue;
                }
                    //add device message channel to map
                map_nuid_dmc.insert(make_pair(found_variable->second.node_uid, tmp_msg_chnl));

                EULAPP_ << boost::str(boost::format("Variable '%1%' allocate device message channel for node '%2%'")%found_variable->second.alias%found_variable->second.node_uid);
            }
        }
    } catch (CException& ex) {
        throw ex;
    }
}

void AbstractExecutionUnit::deallcoateDMC() {
    for(DeviceMessageChannelMapIterator it = map_nuid_dmc.begin(),
        end = map_nuid_dmc.end();
        it != end;
        it++) {
            //deallcoate all device channel
        NetworkBroker::getInstance()->disposeMessageChannel(it->second);
        EULAPP_ << boost::str(boost::format("Deallcaoted device message channel for node '%1%'")%it->first);
    }

        //remove all deallcoated pointer from the map
    map_nuid_dmc.clear();

        //clear also all cache for variable
    map_node_in_var_alias.clear();
    map_node_out_var_alias.clear();
}

    //! allcoate data io driver
void AbstractExecutionUnit::initDataIODriver() throw(CException) {
    int err = 0;
    CDataWrapper *endpoint_configuration = NULL;
        //fetch best three available cds server
    if((err = mds_msg_chnl->getDataDriverBestConfiguration(&endpoint_configuration)) ||
       (endpoint_configuration == NULL)) {
        throw MetadataLoggingCException(getCUID(), err, "Error getting best endpoint configuration", __PRETTY_FUNCTION__);
    }

        //we have result with no error
    if(!endpoint_configuration->hasKey(chaos::DataServiceNodeDefinitionKey::DS_DIRECT_IO_FULL_ADDRESS_LIST) ||
       !endpoint_configuration->isVectorValue(chaos::DataServiceNodeDefinitionKey::DS_DIRECT_IO_FULL_ADDRESS_LIST)) {
        throw MetadataLoggingCException(getCUID(), -10001, "Key not found or invalid for endpoint in CDataWrapper", __PRETTY_FUNCTION__);
    }

        //get the endpoint array
    EULDBG_<< "Scan the result for serverlist";
    std::auto_ptr<CMultiTypeDataArrayWrapper> endpoint_array(endpoint_configuration->getVectorValue(DataServiceNodeDefinitionKey::DS_DIRECT_IO_FULL_ADDRESS_LIST));
    if(endpoint_array->size() == 0) {
        throw MetadataLoggingCException(getCUID(), -10000, "No endpoint available found on metadataserver", __PRETTY_FUNCTION__);

    }

        //set the information
    std::string data_driver_impl = GlobalConfiguration::getInstance()->getOption<std::string>(InitOption::OPT_DATA_IO_IMPL);
    CHAOS_LASSERT_EXCEPTION((data_driver_impl.compare("IODirect") == 0), EULERR_, -2, "Only IODirect implementation is supported")

    data_driver.reset(new IODirectIODriver("IODirectIODriver"));

    IODirectIODriverInitParam init_param;
    std::memset(&init_param, 0, sizeof(IODirectIODriverInitParam));
    init_param.network_broker = NetworkBroker::getInstance();
    init_param.client_instance = NULL;
    init_param.endpoint_instance = NULL;
    ((IODirectIODriver*)data_driver.get())->setDirectIOParam(init_param);
    data_driver->init(NULL);

    for(int idx = 0;
        idx < endpoint_array->size();
        idx++) {
        std::string server = endpoint_array->getStringElementAtIndex(idx);
        EULDBG_<< "Add " << server << " to server list";
        ((IODirectIODriver*)data_driver.get())->addServerURL(server);
    }
}

void AbstractExecutionUnit::deinitDataIODriver() {
    if(data_driver.get() == NULL) return;
    CHAOS_NOT_THROW(data_driver->deinit(););
    data_driver.reset();
}

void AbstractExecutionUnit::addVariableInCache(VariableParameterDefinition& variable) {
        //check if we have an input variable
    if((variable.direction == DataType::Input) ||
       (variable.direction == DataType::Bidirectional)) {
            //for input variabl ewe create the key of map with the dataset to fetch
        const std::string dataset_key = variable.node_uid + DataPackPrefixID::OUTPUT_DATASE_PREFIX;

            //check for cache class for node uid
        if(map_node_in_var_alias.count(dataset_key) == 0) {
            EULAPP_<< "Create new sets for associate input variable alias to the node uid dataset: " << dataset_key;

                //prepare cache for new node assocaited to the input variable
            map_node_in_var_alias.insert(make_pair(dataset_key,
                                                   std::set<std::string>()));
        }
            //add variable to the set of associated alias to node uid
        map_node_in_var_alias[dataset_key].insert(variable.alias);

        EULAPP_<< "Create cache for input variable alias "<<variable.alias;
        input_variable_cache->addAttribute(variable.alias, 0, variable.type);
    }

        //check if we have an output variable
    if((variable.direction == DataType::Output) ||
       (variable.direction == DataType::Bidirectional)) {
            //for output variable the key of the map is the node to control
            //check for cache class for node uid
        if(map_node_out_var_alias.count(variable.node_uid) == 0) {
            EULAPP_<< "Create new sets for associate output variable alias to the node uid dataset: " << variable.node_uid;

                //prepare cache for new node assocaited to the input variable
            map_node_out_var_alias.insert(make_pair(variable.node_uid,
                                                    std::set<std::string>()));
        }
            //add variable to the set of associated alias to node uid
        map_node_out_var_alias[variable.node_uid].insert(variable.alias);

        EULAPP_<< "Create cache for output variable alias "<<variable.alias;
        output_variable_cache->addAttribute(variable.alias, 0, variable.type);
    }
}

void AbstractExecutionUnit::fetchInputVariableValues() {
    std::auto_ptr<CDataWrapper> tmp_dataset;
    for(NodeUIDToVariableMapIterator it = map_node_in_var_alias.begin(),
        end = map_node_in_var_alias.end();
        it != end;
        it++) {

        const std::string node_uid_ds = it->first;

            //fetch live data
        char * data_received = data_driver->retriveRawData(node_uid_ds);
        if(data_received == NULL) continue;
        tmp_dataset.reset(new CDataWrapper(data_received));
        free(data_received);

            //now fetch value for every variable alias
        for(SetVariableAliasIterator var_alias_it = it->second.begin(),
            var_alias_end = it->second.end();
            var_alias_it != var_alias_end;
            var_alias_it++){
            if(variable_map.count(*var_alias_it) == 0) continue;
                // get value for attribute associated to the alias
            VariableParameterDefinition& variable_definition = variable_map[*var_alias_it];

            if(tmp_dataset->hasKey(variable_definition.node_dataset_attribute)) {
                const char * raw_value_buffer = tmp_dataset->getRawValuePtr(variable_definition.node_dataset_attribute);
                uint32_t raw_value_buffer_len = tmp_dataset->getValueSize(variable_definition.node_dataset_attribute);
                    //update cached value
                input_variable_cache->setValueForAttribute(variable_definition.alias,
                                                           raw_value_buffer,
                                                           raw_value_buffer_len);
            }
        }
    }
}

void AbstractExecutionUnit::applyModificationOnOutputNode() {
        //scan all node to control
    for(NodeUIDToVariableMapIterator it = map_node_out_var_alias.begin(),
        end = map_node_out_var_alias.end();
        it != end;
        it++) {
        CDataWrapper node_change_set;
            //for every node we sent an unique message with all modificaiton
        const std::string node_to_control = it->first;


            //scan variable to get the attribute to configure on remote node
        for(SetVariableAliasIterator var_alias_it = it->second.begin(),
            var_alias_end = it->second.end();
            var_alias_it != var_alias_end;
            var_alias_it++){
            if(variable_map.count(*var_alias_it) == 0) continue;
                // get value for attribute associated to the alias
            VariableParameterDefinition& variable_definition = variable_map[*var_alias_it];

                //fill CDataWrapper with key and value of the attribute
            AttributeValue *cached_value = output_variable_cache->getValueSettingByName(variable_definition.alias);

                //write cached data to cdata wrapper
            cached_value->writeToCDataWrapper(node_change_set);
        }

            //submit change-set to the node
        map_nuid_dmc[node_to_control]->setAttributeValue(node_change_set);
    }
}

#define VAR_IN_CDW(x,y)\
x.addStringValue(ExecutionUnitNodeDefinitionKey::EXECUTION_UNIT_VAR_ALIAS, y->second.alias);\
x.addStringValue(ExecutionUnitNodeDefinitionKey::EXECUTION_UNIT_VAR_DESCRIPTION, y->second.description);\
x.addInt32Value(ExecutionUnitNodeDefinitionKey::EXECUTION_UNIT_VAR_TYPE, y->second.type);\
x.addInt32Value(ExecutionUnitNodeDefinitionKey::EXECUTION_UNIT_VAR_DIRECTION, y->second.direction);\
x.addBoolValue(ExecutionUnitNodeDefinitionKey::EXECUTION_UNIT_VAR_MANDATORY, y->second.mandatory);\
x.addStringValue(ExecutionUnitNodeDefinitionKey::EXECUTION_UNIT_VAR_DATASET_ATTRIBUTE, y->second.node_dataset_attribute);\
x.addStringValue(ExecutionUnitNodeDefinitionKey::EXECUTION_UNIT_VAR_NODE_UID, y->second.node_uid);

/*!
 Define the control unit DataSet and Action into
 a CDataWrapper
 */
void AbstractExecutionUnit::_defineActionAndDataset(chaos_data::CDataWrapper& setup_configuration) throw(CException) {
        //call superclass method
    RTAbstractControlUnit::_defineActionAndDataset(setup_configuration);
        //add description
    setup_configuration.addStringValue(ExecutionUnitNodeDefinitionKey::EXECUTION_UNIT_DESCRIPTION, eu_description);
        //check for variable
    if(variable_map.size()) {
            //complete the dataset includigin the variable for the execution unit
        for(VPDMapIterator it = variable_map.begin(),
            end = variable_map.end();
            it != end;
            it++) {
                //describe the variable
            CDataWrapper temp_var_description;

                //conver var to cdata wrapper
            VAR_IN_CDW(temp_var_description, it)

                //append description to array
            setup_configuration.appendCDataWrapperToArray(temp_var_description);
        }
            //finalize the array
        setup_configuration.finalizeArrayForKey(ExecutionUnitNodeDefinitionKey::EXECUTION_UNIT_VARIABLE_LIST);
    }
}

    //! inherited method
void AbstractExecutionUnit::unitInit() throw(CException) {
    input_variable_cache.reset(new AttributeCache());
    common::utility::InizializableService::initImplementation(input_variable_cache.get(), NULL, "AttributeCache", __PRETTY_FUNCTION__);

    output_variable_cache.reset(new AttributeCache());
    common::utility::InizializableService::initImplementation(output_variable_cache.get(), NULL, "AttributeCache", __PRETTY_FUNCTION__);

        //allocate device channel for output variable
    mds_msg_chnl = NetworkBroker::getInstance()->getMetadataserverMessageChannel();
    if(mds_msg_chnl == NULL) throw MetadataLoggingCException(getCUID(), -1, "No metadata server channel got", __PRETTY_FUNCTION__);

        //initilize the devices message channel
    EULAPP_ << "Complete variable with initialization data";
        //get a copy for last received initilization package
    CDataWrapper last_init_cdw;
        //get last initilization pack
    AbstractControlUnit::copyInitConfiguraiton(last_init_cdw);
        //allocate channel for output controlled devices
    completeVaribleAndAllocateDMC(last_init_cdw);

    EULAPP_ << "Allocate DataIO driver for fetch input variable value";
    initDataIODriver();
}

    //! inherited method
void AbstractExecutionUnit::unitStart() throw(CException) {

}

    //! inherited method
void AbstractExecutionUnit::unitRun() throw(CException) {
        //get temp pointe to step timestamp
    uint64_t *step_ts = timestamp_acq_cached_value->getValuePtr<uint64_t>();
        //fetch all input variable
    fetchInputVariableValues();
        //execute algorithm step
    executeAlgorithmStep(((last_execution_ts==0)?0:(*step_ts-last_execution_ts)));
        //apply the modification and command
    applyModificationOnOutputNode();
        //keep track of current timestamp
    last_execution_ts = *step_ts;
}

    //! inherited method
void AbstractExecutionUnit::unitStop() throw(CException) {
}

    //! inherited method
void AbstractExecutionUnit::unitDeinit() throw(CException) {
    EULAPP_ << "Deallocate DataIO driver for fetch input variable value";
    initDataIODriver();
        //initilize the devices message channel
    EULAPP_ << "Deinitialize all device message channels";
        //deallcoate all message channel
    deallcoateDMC();
        //dispose mds message channel
    if(mds_msg_chnl){NetworkBroker::getInstance()->disposeMessageChannel(mds_msg_chnl);}
    
    CHAOS_NOT_THROW(common::utility::InizializableService::initImplementation(input_variable_cache.get(), NULL, "AttributeCache", __PRETTY_FUNCTION__););
    input_variable_cache.reset();
    
    CHAOS_NOT_THROW(common::utility::InizializableService::initImplementation(output_variable_cache.get(), NULL, "AttributeCache", __PRETTY_FUNCTION__););
    output_variable_cache.reset();
}
