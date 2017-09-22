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

#include <chaos/cu_toolkit/control_manager/ControlUnitApiInterface.h>
#include <chaos/cu_toolkit/control_manager/AbstractControlUnit.h>

using namespace chaos::cu::control_manager;

#define GET_OR_RETURN(x)\
LKControlUnitInstancePtrReadLock rl = control_unit_pointer.getReadLockObject();\
if(control_unit_pointer() == NULL) return x;

ControlUnitApiInterface::ControlUnitApiInterface():
dummy_str("No CU Set"){
    control_unit_pointer() = NULL;
}

ControlUnitApiInterface::~ControlUnitApiInterface(){}

bool ControlUnitApiInterface::_variantHandler(const std::string& attribute_name,
                                              const chaos::common::data::CDataVariant& value) {
    if(attribute_handler_functor == NULL) return true;
    
    GET_OR_RETURN(false);
    return attribute_handler_functor(control_unit_pointer()->getCUID(),
                                     attribute_name,
                                     value);
    
}

void ControlUnitApiInterface::fireEvent(const ControlUnitProxyEvent event_type) {
    if(event_handler == NULL) return;
    GET_OR_RETURN();
    event_handler(control_unit_pointer()->getCUID(),
                  event_type);
}

void ControlUnitApiInterface::setAttributeHandlerFunctor(AttributeHandlerFunctor _functor) {
    attribute_handler_functor = _functor;
}

void ControlUnitApiInterface::setEventHandlerFunctor(EventHandlerFunctor _functor) {
    event_handler = _functor;
}


const std::string& ControlUnitApiInterface::getCUID() {
    GET_OR_RETURN(dummy_str);
    return control_unit_pointer()->getCUID();
}

const std::string& ControlUnitApiInterface::getCUParam() {
    GET_OR_RETURN(dummy_str);
    return control_unit_pointer()->getCUParam();
}

const std::string& ControlUnitApiInterface::getCUType() {
    GET_OR_RETURN(dummy_str);
    return control_unit_pointer()->getCUType();
}

AttributeSharedCacheWrapper * const ControlUnitApiInterface::getAttributeCache() {
    GET_OR_RETURN(NULL);
    return control_unit_pointer()->getAttributeCache();
}

bool ControlUnitApiInterface::enableHandlerOnInputAttributeName(const std::string& attribute_name) {
    GET_OR_RETURN(false);
    return control_unit_pointer()->addVariantHandlerOnInputAttributeName<ControlUnitApiInterface>(this,
                                                                                                  &ControlUnitApiInterface::_variantHandler,
                                                                                                  attribute_name);
}

bool ControlUnitApiInterface::removeHandlerOnInputAttributeName(const std::string& attribute_name) {
    GET_OR_RETURN(false);
    return control_unit_pointer()->removeHandlerOnAttributeName(attribute_name);
}

void ControlUnitApiInterface::pushOutputDataset() {
    GET_OR_RETURN();
    control_unit_pointer()->pushOutputDataset();
}

void ControlUnitApiInterface::pushInputDataset() {
    GET_OR_RETURN();
    control_unit_pointer()->pushInputDataset();
}

void ControlUnitApiInterface::addAttributeToDataSet(const std::string& attribute_name,
                                                    const std::string& attribute_description,
                                                    DataType::DataType attribute_type,
                                                    DataType::DataSetAttributeIOAttribute attribute_direction,
                                                    uint32_t maxSize) {
    GET_OR_RETURN();
    control_unit_pointer()->addAttributeToDataSet(attribute_name,
                                                  attribute_description,
                                                  attribute_type,
                                                  attribute_direction,
                                                  maxSize);
}
void ControlUnitApiInterface::addBinaryAttributeAsSubtypeToDataSet(const std::string& attribute_name,
                                                                   const std::string& attribute_description,
                                                                   DataType::BinarySubtype               subtype,
                                                                   int32_t    cardinality,
                                                                   DataType::DataSetAttributeIOAttribute attribute_direction) {
    GET_OR_RETURN();
    control_unit_pointer()->addBinaryAttributeAsSubtypeToDataSet(attribute_name,
                                                                 attribute_description,
                                                                 subtype,
                                                                 cardinality,
                                                                 attribute_direction);
}
void ControlUnitApiInterface::addBinaryAttributeAsSubtypeToDataSet(const std::string&            attribute_name,
                                                                   const std::string&            attribute_description,
                                                                   const std::vector<int32_t>&   subtype_list,
                                                                   int32_t                       cardinality,
                                                                   DataType::DataSetAttributeIOAttribute attribute_direction) {
    GET_OR_RETURN();
    control_unit_pointer()->addBinaryAttributeAsSubtypeToDataSet(attribute_name,
                                                                 attribute_description,
                                                                 subtype_list,
                                                                 cardinality,
                                                                 attribute_direction);
}

void ControlUnitApiInterface::addBinaryAttributeAsMIMETypeToDataSet(const std::string& attribute_name,
                                                                    const std::string& attribute_description,
                                                                    const std::string& mime_type,
                                                                    DataType::DataSetAttributeIOAttribute attribute_direction) {
    GET_OR_RETURN();
    control_unit_pointer()->addBinaryAttributeAsMIMETypeToDataSet(attribute_name,
                                                                  attribute_description,
                                                                  mime_type,
                                                                  attribute_direction);
}

void ControlUnitApiInterface::getDatasetAttributesName(ChaosStringVector& attributesName) {
    GET_OR_RETURN();
    control_unit_pointer()->getDatasetAttributesName(attributesName);
}


void ControlUnitApiInterface::getDatasetAttributesName(DataType::DataSetAttributeIOAttribute directionType,
                                                       ChaosStringVector& attributesName) {
    GET_OR_RETURN();
    control_unit_pointer()->getDatasetAttributesName(directionType,
                                                     attributesName);
}

void ControlUnitApiInterface::getAttributeDescription(const std::string& attributesName,
                                                      std::string& attributeDescription) {
    GET_OR_RETURN();
    control_unit_pointer()->getAttributeDescription(attributesName,
                                                    attributeDescription);
}

int ControlUnitApiInterface::getAttributeRangeValueInfo(const std::string& attribute_name,
                                                        chaos::common::data::RangeValueInfo& range_info) {
    GET_OR_RETURN(-1);
    return control_unit_pointer()->getAttributeRangeValueInfo(attribute_name,
                                                              range_info);
}

void ControlUnitApiInterface::setAttributeRangeValueInfo(const std::string& attribute_name,
                                                         chaos::common::data::RangeValueInfo& range_info) {
    GET_OR_RETURN();
    control_unit_pointer()->setAttributeRangeValueInfo(attribute_name,
                                                       range_info);
}

int ControlUnitApiInterface::getAttributeDirection(const std::string& attribute_name,
                                                   DataType::DataSetAttributeIOAttribute& directionType) {
    GET_OR_RETURN(-1);
    return control_unit_pointer()->getAttributeDirection(attribute_name,
                                                         directionType);
}

void ControlUnitApiInterface::addStateVariable(StateVariableType variable_type,
                                               const std::string& state_variable_name,
                                               const std::string& state_variable_description) {
    GET_OR_RETURN();
    control_unit_pointer()->addStateVariable(variable_type,
                                             state_variable_name,
                                             state_variable_description);
}

void ControlUnitApiInterface::setStateVariableSeverity(StateVariableType variable_type,
                                                       const chaos::common::alarm::MultiSeverityAlarmLevel state_variable_severity) {
    GET_OR_RETURN();
    control_unit_pointer()->setStateVariableSeverity(variable_type,
                                                     state_variable_severity);
}

bool ControlUnitApiInterface::setStateVariableSeverity(StateVariableType variable_type,
                                                       const std::string& state_variable_name,
                                                       const chaos::common::alarm::MultiSeverityAlarmLevel state_variable_severity) {
    GET_OR_RETURN(false);
    return control_unit_pointer()->setStateVariableSeverity(variable_type,
                                                            state_variable_name,
                                                            state_variable_severity);
}

bool ControlUnitApiInterface::setStateVariableSeverity(StateVariableType variable_type,
                                                       const unsigned int state_variable_ordered_id,
                                                       const chaos::common::alarm::MultiSeverityAlarmLevel state_variable_severity) {
    GET_OR_RETURN(false);
    return control_unit_pointer()->setStateVariableSeverity(variable_type,
                                                            state_variable_ordered_id,
                                                            state_variable_severity);
}


bool ControlUnitApiInterface::getStateVariableSeverity(StateVariableType variable_type,
                                                       const std::string& state_variable_name,
                                                       chaos::common::alarm::MultiSeverityAlarmLevel& state_variable_severity) {
    GET_OR_RETURN(false);
    return control_unit_pointer()->getStateVariableSeverity(variable_type,
                                                            state_variable_name,
                                                            state_variable_severity);
}


bool ControlUnitApiInterface::getStateVariableSeverity(StateVariableType variable_type,
                                                       const unsigned int state_variable_ordered_id,
                                                       chaos::common::alarm::MultiSeverityAlarmLevel& state_variable_severity) {
    GET_OR_RETURN(false);
    return control_unit_pointer()->getStateVariableSeverity(variable_type,
                                                            state_variable_ordered_id,
                                                            state_variable_severity);
}

void ControlUnitApiInterface::setBusyFlag(bool state) {
    GET_OR_RETURN();
    control_unit_pointer()->setBusyFlag(state);
}

const bool ControlUnitApiInterface::getBusyFlag() const {
    GET_OR_RETURN(false);
    return control_unit_pointer()->getBusyFlag();
}

