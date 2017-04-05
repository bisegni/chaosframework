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
using namespace chaos::cu::data_manager;
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

void AbstractExecutionUnit::_defineActionAndDataset(chaos_data::CDataWrapper& setup_configuration) throw(CException) {
    //call superclass method
    RTAbstractControlUnit::_defineActionAndDataset(setup_configuration);
    //add description
    if(eu_description.size()){setup_configuration.addStringValue(ExecutionUnitNodeDefinitionKey::EXECUTION_UNIT_DESCRIPTION, eu_description);}
}

//! inherited method
void AbstractExecutionUnit::unitInit() throw(CException) {
    executeAlgorithmLaunch();
}

//! inherited method
void AbstractExecutionUnit::unitStart() throw(CException) {
    executeAlgorithmStart();
}

//! inherited method
void AbstractExecutionUnit::unitRun() throw(CException) {
    //get temp pointe to step timestamp
    uint64_t *step_ts = timestamp_acq_cached_value->getValuePtr<uint64_t>();
    //execute algorithm step
    executeAlgorithmStep(((last_execution_ts==0)?0:(*step_ts-last_execution_ts)));
    //keep track of current timestamp
    last_execution_ts = *step_ts;
}

//! inherited method
void AbstractExecutionUnit::unitStop() throw(CException) {
    executeAlgorithmStop();
}

//! inherited method
void AbstractExecutionUnit::unitDeinit() throw(CException) {
    executeAlgorithmEnd();
}

int AbstractExecutionUnit::performQuery(chaos::common::io::QueryCursor **cursor,
                                        const std::string& node_id,
                                        KeyDataStorageDomain dataset_domain,
                                        const uint64_t start_ts,
                                        const uint64_t end_ts,
                                        const uint32_t page_len) {
    CHAOS_ASSERT(key_data_storage.get());
    int err = key_data_storage->performGeneralQuery(cursor,
                                                    node_id,
                                                    dataset_domain,
                                                    start_ts,
                                                    end_ts);
    if(!err || *cursor) {
        (*cursor)->setPageDimension(page_len);
    }
    return err;
}

void AbstractExecutionUnit::releseQuery(chaos::common::io::QueryCursor *cursor) {
    key_data_storage->releseQuery(cursor);
}
