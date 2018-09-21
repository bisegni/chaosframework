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
using namespace chaos::cu::data_manager;
using namespace chaos::cu::control_manager;

using namespace boost;
using namespace boost::chrono;

#define EULAPP_ INFO_LOG_1_P(AbstractExecutionUnit, getCUInstance())
#define EULDBG_ DBG_LOG_1_P(AbstractExecutionUnit, getCUInstance())
#define EULERR_ ERR_LOG_1_P(AbstractExecutionUnit, getCUInstance())

AbstractExecutionUnit::AbstractExecutionUnit(const std::string& _execution_unit_subtype,
                                             const std::string& _execution_unit_id,
                                             const std::string& _execution_unit_param):
RTAbstractControlUnit(_execution_unit_subtype,
                      _execution_unit_id,
                      _execution_unit_param),
last_execution_ts(0){}

/*!
 Parametrized constructor
 \param _execution_unit_id unique id for the control unit
 \param _execution_unit_drivers driver information
 */
AbstractExecutionUnit::AbstractExecutionUnit(const std::string& _execution_unit_subtype,
                                             const std::string& _execution_unit_id,
                                             const std::string& _execution_unit_param,
                                             const ControlUnitDriverList& _execution_unit_drivers):
RTAbstractControlUnit(_execution_unit_subtype,
                      _execution_unit_id,
                      _execution_unit_param,
                      _execution_unit_drivers),
last_execution_ts(0){}

AbstractExecutionUnit::~AbstractExecutionUnit() {}

//!redefine private for protection
AbstractSharedDomainCache* AbstractExecutionUnit::_getAttributeCache() {
    return RTAbstractControlUnit::_getAttributeCache();
}

#pragma Execution Uni tAPI

void AbstractExecutionUnit::setExecutionUnitDescription(const std::string& description) {
    eu_description = description;
}


void AbstractExecutionUnit::addExecutionUnitAttributeToDataSet(const std::string& attribute_name,
                                                               const std::string& attribute_description,
                                                               DataType::DataType attribute_type,
                                                               DataType::DataSetAttributeIOAttribute attribute_direction,
                                                               uint32_t maxSize) {
    DatasetDB::addAttributeToDataSet(attribute_name,
                                     attribute_description,
                                     attribute_type,
                                     attribute_direction,
                                     maxSize);
}


void AbstractExecutionUnit::addBinaryAttributeAsSubtypeToDataSet(const std::string&                    attribute_name,
                                                                 const std::string&                    attribute_description,
                                                                 DataType::BinarySubtype               subtype,
                                                                 int32_t                               cardinality,
                                                                 DataType::DataSetAttributeIOAttribute attribute_direction) {
    DatasetDB::addBinaryAttributeAsSubtypeToDataSet(attribute_name,
                                                    attribute_description,
                                                    subtype,
                                                    cardinality,
                                                    attribute_direction);
}


void AbstractExecutionUnit::addBinaryAttributeAsSubtypeToDataSet(const std::string&                     attribute_name,
                                                                 const std::string&                     attribute_description,
                                                                 const std::vector<int32_t>&            subtype_list,
                                                                 int32_t                                cardinality,
                                                                 DataType::DataSetAttributeIOAttribute  attribute_direction) {
    DatasetDB::addBinaryAttributeAsSubtypeToDataSet(attribute_name,
                                                    attribute_description,
                                                    subtype_list,
                                                    cardinality,
                                                    attribute_direction);
}

void AbstractExecutionUnit::addBinaryAttributeAsMIMETypeToDataSet(const std::string& attribute_name,
                                                                  const std::string& attribute_description,
                                                                  std::string mime_type,
                                                                  DataType::DataSetAttributeIOAttribute attribute_direction){
    DatasetDB::addBinaryAttributeAsMIMETypeToDataSet(attribute_name,
                                                     attribute_description,
                                                     mime_type,
                                                     attribute_direction);
}

CDataVariant AbstractExecutionUnit::getOutputAttributeValue(const std::string& attribute_name) {
    CHAOS_ASSERT(attribute_value_shared_cache)
    CDataVariant result;
    AttributeValue *cached_value = NULL;
    cached_value = attribute_value_shared_cache->getAttributeValue(DOMAIN_OUTPUT,
                                                                   attribute_name);
    return cached_value?cached_value->getAsVariant():CDataVariant();
}

CDataVariant AbstractExecutionUnit::getInputAttributeValue(const std::string& attribute_name) {
    CHAOS_ASSERT(attribute_value_shared_cache)
    CDataVariant result;
    AttributeValue *cached_value = NULL;
    cached_value = attribute_value_shared_cache->getAttributeValue(DOMAIN_INPUT,
                                                                   attribute_name);
    return cached_value?cached_value->getAsVariant():CDataVariant();
}

void AbstractExecutionUnit::setOutputAttributeValue(const std::string& attribute_name,
                                                    const CDataVariant& attribute_value) {
    CHAOS_ASSERT(attribute_value_shared_cache)
    CDataVariant result;
    AttributeValue *cached_value = NULL;
    cached_value = attribute_value_shared_cache->getAttributeValue(DOMAIN_OUTPUT,
                                                                   attribute_name);
    cached_value->setValue(attribute_value);
}

void AbstractExecutionUnit::_defineActionAndDataset(chaos_data::CDataWrapper& setup_configuration) {
    //call superclass method
    RTAbstractControlUnit::_defineActionAndDataset(setup_configuration);
    //add description
    if(eu_description.size()){setup_configuration.addStringValue(ExecutionUnitNodeDefinitionKey::EXECUTION_UNIT_DESCRIPTION, eu_description);}
}

void AbstractExecutionUnit::unitInit() {
    
    executeAlgorithmLaunch();
}

void AbstractExecutionUnit::unitStart() {
    executeAlgorithmStart();
}

void AbstractExecutionUnit::unitRun() {
    //get temp pointe to step timestamp
    uint64_t *step_ts = timestamp_acq_cached_value->getValuePtr<uint64_t>();
    //execute algorithm step
    executeAlgorithmStep(((last_execution_ts==0)?0:(*step_ts-last_execution_ts)));
    //keep track of current timestamp
    last_execution_ts = *step_ts;
}

void AbstractExecutionUnit::unitStop() {
    executeAlgorithmStop();
}

void AbstractExecutionUnit::unitDeinit() {
    executeAlgorithmEnd();
}

int AbstractExecutionUnit::performLiveFetch(const chaos::cu::data_manager::KeyDataStorageDomain dataset_domain,
                                            chaos::common::data::CDWShrdPtr& found_dataset) {
    return key_data_storage->performLiveFetch(dataset_domain,
                                              found_dataset);
}

int AbstractExecutionUnit::performLiveFetch(const ChaosStringVector& node_uid,
                                            const chaos::cu::data_manager::KeyDataStorageDomain dataset_domain,
                                            chaos::common::data::VectorCDWShrdPtr& found_dataset) {
    return key_data_storage->performLiveFetch(node_uid,
                                              dataset_domain,
                                              found_dataset);
}

int AbstractExecutionUnit::performQuery(chaos::common::io::QueryCursor **cursor,
                                        const std::string& node_id,
                                        KeyDataStorageDomain dataset_domain,
                                        const uint64_t start_ts,
                                        const uint64_t end_ts,
                                        const ChaosStringSet& meta_tags,
                                        const uint32_t page_len) {
    CHAOS_ASSERT(key_data_storage.get());
    int err = key_data_storage->performGeneralQuery(cursor,
                                                    node_id,
                                                    dataset_domain,
                                                    start_ts,
                                                    end_ts,
                                                    meta_tags);
    if(!err || *cursor) {
        (*cursor)->setPageDimension(page_len);
    }
    return err;
}

void AbstractExecutionUnit::releseQuery(chaos::common::io::QueryCursor *cursor) {
    key_data_storage->releseQuery(cursor);
}
