/*
 *	ProxyControlUnit.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 24/01/2017 INFN, National Institute of Nuclear Physics
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

#include <chaos/cu_toolkit/control_manager/ProxyControlUnit.h>

#include <chaos/common/utility/TimingUtil.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::exception;
using namespace chaos::common::data::cache;

using namespace chaos::cu;
using namespace chaos::cu::control_manager;

using namespace boost;
using namespace boost::chrono;

#define PRXCUINFO   INFO_LOG(ProxyControlUnit)
#define PRXCUDBG    DBG_LOG(ProxyControlUnit)
#define PRXCUERR    ERR_LOG(ProxyControlUnit)

ProxyControlUnit::ProxyControlUnit(const std::string& _control_unit_id,
                                   const std::string& _control_unit_param):
AbstractControlUnit(CUType::RTCU,
                    _control_unit_id,
                    _control_unit_param),
ControlUnitApiInterface(this){
    attribute_value_shared_cache = new AttributeValueSharedCache();
}

/*!
 Parametrized constructor
 \param _control_unit_id unique id for the control unit
 \param _control_unit_drivers driver information
 */
ProxyControlUnit::ProxyControlUnit(const std::string& _control_unit_id,
                                   const std::string& _control_unit_param,
                                   const ControlUnitDriverList& _control_unit_drivers):
AbstractControlUnit(CUType::RTCU,
                    _control_unit_id,
                    _control_unit_param,
                    _control_unit_drivers),
ControlUnitApiInterface(this) {
    attribute_value_shared_cache = new AttributeValueSharedCache();
}


ProxyControlUnit::ProxyControlUnit(const std::string& _alternate_type,
                                   const std::string& _control_unit_id,
                                   const std::string& _control_unit_param):
AbstractControlUnit(_alternate_type,
                    _control_unit_id,
                    _control_unit_param),
ControlUnitApiInterface(this) {
    attribute_value_shared_cache = new AttributeValueSharedCache();
}

ProxyControlUnit::ProxyControlUnit(const std::string& _alternate_type,
                                   const std::string& _control_unit_id,
                                   const std::string& _control_unit_param,
                                   const ControlUnitDriverList& _control_unit_drivers):
AbstractControlUnit(_alternate_type,
                    _control_unit_id,
                    _control_unit_param,
                    _control_unit_drivers),
ControlUnitApiInterface(this) {
    attribute_value_shared_cache = new AttributeValueSharedCache();
}

ProxyControlUnit::~ProxyControlUnit() {
    //release attribute shared cache
    if(attribute_value_shared_cache) {
        delete(attribute_value_shared_cache);
    }
}

/*
 fill the CDataWrapper with AbstractCU system configuration, this method
 is called after getStartConfiguration directly by sandbox. in this method
 are defined the action for the input element of the dataset
 */
void ProxyControlUnit::_defineActionAndDataset(CDataWrapper& setup_configuration)  throw(CException) {
    AbstractControlUnit::_defineActionAndDataset(setup_configuration);
    //add the scekdule dalay for the sandbox
}

/*!
 Init the  RT Control Unit scheduling for device
 */
void ProxyControlUnit::init(void *initData) throw(CException) {
    //call parent impl
    AbstractControlUnit::init(initData);
    PRXCUINFO << "Initializing shared attribute cache " << DatasetDB::getDeviceID();
    InizializableService::initImplementation((AttributeValueSharedCache*)attribute_value_shared_cache, (void*)NULL, "attribute_value_shared_cache", __PRETTY_FUNCTION__);
}

/*!
 Starto the  Control Unit scheduling for device
 */
void ProxyControlUnit::start() throw(CException) {
    //call parent impl
    AbstractControlUnit::start();
}

/*!
 Stop the Custom Control Unit scheduling for device
 */
void ProxyControlUnit::stop() throw(CException) {
    //call parent impl
    AbstractControlUnit::stop();
}

/*!
 Init the  RT Control Unit scheduling for device
 */
void ProxyControlUnit::deinit() throw(CException) {
    //call parent impl
    AbstractControlUnit::deinit();
    
    PRXCUINFO << "Deinitializing shared attribute cache " << DatasetDB::getDeviceID();
    InizializableService::deinitImplementation((AttributeValueSharedCache*)attribute_value_shared_cache, "attribute_value_shared_cache", __PRETTY_FUNCTION__);
}

/*!
 Event for update some CU configuration
 */
CDataWrapper* ProxyControlUnit::updateConfiguration(CDataWrapper* update_pack, bool& detach_param) throw (CException) {
    CDataWrapper *result = AbstractControlUnit::updateConfiguration(update_pack, detach_param);
    std::auto_ptr<CDataWrapper> cu_properties;
    CDataWrapper *cu_property_container = NULL;
    if(update_pack->hasKey(ControlUnitDatapackSystemKey::THREAD_SCHEDULE_DELAY)){
        cu_property_container = update_pack;
    } else if(update_pack->hasKey("property_abstract_control_unit") &&
              update_pack->isCDataWrapperValue("property_abstract_control_unit")){
        cu_properties.reset(update_pack->getCSDataValue("property_abstract_control_unit"));
        cu_property_container = cu_properties.get();
    }
    return result;
}

////---------------definition api-------------
//
//void ProxyControlUnit::addAttributeToDataSet(const std::string& attribute_name,
//                                             const std::string& attribute_description,
//                                             DataType::DataType attribute_type,
//                                             DataType::DataSetAttributeIOAttribute attribute_direction,
//                                             uint32_t maxSize) {
//    AbstractControlUnit::addAttributeToDataSet(attribute_name,
//                                               attribute_description,
//                                               attribute_type,
//                                               attribute_direction,
//                                               maxSize);
//}
//
//void ProxyControlUnit::addBinaryAttributeAsSubtypeToDataSet(const std::string& attribute_name,
//                                                            const std::string& attribute_description,
//                                                            DataType::BinarySubtype               subtype,
//                                                            int32_t    cardinality,
//                                                            DataType::DataSetAttributeIOAttribute attribute_direction) {
//    AbstractControlUnit::addBinaryAttributeAsSubtypeToDataSet(attribute_name,
//                                                              attribute_description,
//                                                              subtype,
//                                                              cardinality,
//                                                              attribute_direction);
//}
//
//void ProxyControlUnit::addBinaryAttributeAsSubtypeToDataSet(const std::string&            attribute_name,
//                                                            const std::string&            attribute_description,
//                                                            const std::vector<int32_t>&   subtype_list,
//                                                            int32_t                       cardinality,
//                                                            DataType::DataSetAttributeIOAttribute attribute_direction){
//    AbstractControlUnit::addBinaryAttributeAsSubtypeToDataSet(attribute_name,
//                                                              attribute_description,
//                                                              subtype_list,
//                                                              cardinality,
//                                                              attribute_direction);
//}
//
//void ProxyControlUnit::addBinaryAttributeAsMIMETypeToDataSet(const std::string& attribute_name,
//                                                             const std::string& attribute_description,
//                                                             std::string mime_type,
//                                                             DataType::DataSetAttributeIOAttribute attribute_direction) {
//    AbstractControlUnit::addBinaryAttributeAsMIMETypeToDataSet(attribute_name,
//                                                               attribute_description,
//                                                               mime_type,
//                                                               attribute_direction);
//}
//
//void ProxyControlUnit::getDatasetAttributesName(vector<string>& attributesName){
//    AbstractControlUnit::getDatasetAttributesName(attributesName);
//}
//
//void ProxyControlUnit::getDatasetAttributesName(DataType::DataSetAttributeIOAttribute directionType,
//                                                vector<string>& attributesName){
//    AbstractControlUnit::getDatasetAttributesName(directionType,
//                                                  attributesName);
//}
//
//void ProxyControlUnit::getAttributeDescription(const string& attributesName,
//                                               string& attributeDescription){
//    AbstractControlUnit::getAttributeDescription(attributesName,
//                                                 attributeDescription);
//}
//
//int ProxyControlUnit::getAttributeRangeValueInfo(const string& attributesName,
//                                                 chaos_data::RangeValueInfo& rangeInfo){
//    return AbstractControlUnit::getAttributeRangeValueInfo(attributesName,
//                                                           rangeInfo);
//}
//
//void ProxyControlUnit::setAttributeRangeValueInfo(const string& attributesName,
//                                                  chaos_data::RangeValueInfo& rangeInfo){
//    AbstractControlUnit::setAttributeRangeValueInfo(attributesName,
//                                                    rangeInfo);
//}
//
//int ProxyControlUnit::getAttributeDirection(const string& attributesName,
//                                            DataType::DataSetAttributeIOAttribute& directionType){
//    return AbstractControlUnit::getAttributeDirection(attributesName,
//                                                      directionType);
//}
//
//void ProxyControlUnit::addStateVariable(chaos::cu::control_manager::StateVariableType variable_type,
//                                        const std::string& state_variable_name,
//                                        const std::string& state_variable_description){
//    AbstractControlUnit::addStateVariable(variable_type,
//                                          state_variable_name,
//                                          state_variable_description);
//}
//
//void ProxyControlUnit::setStateVariableSeverity(chaos::cu::control_manager::StateVariableType variable_type,
//                                                const common::alarm::MultiSeverityAlarmLevel state_variable_severity){
//    AbstractControlUnit::setStateVariableSeverity(variable_type,
//                                                  state_variable_severity);
//}
//
//bool ProxyControlUnit::setStateVariableSeverity(chaos::cu::control_manager::StateVariableType variable_type,
//                                                const std::string& state_variable_name,
//                                                const common::alarm::MultiSeverityAlarmLevel state_variable_severity){
//    return AbstractControlUnit::setStateVariableSeverity(variable_type,
//                                                         state_variable_name,
//                                                         state_variable_severity);
//}
//
//bool ProxyControlUnit::setStateVariableSeverity(chaos::cu::control_manager::StateVariableType variable_type,
//                                                const unsigned int state_variable_ordered_id,
//                                                const common::alarm::MultiSeverityAlarmLevel state_variable_severity){
//    return AbstractControlUnit::setStateVariableSeverity(variable_type,
//                                                         state_variable_ordered_id,
//                                                         state_variable_severity);
//    
//}
//
//bool ProxyControlUnit::getStateVariableSeverity(chaos::cu::control_manager::StateVariableType variable_type,
//                                                const std::string& state_variable_name,
//                                                common::alarm::MultiSeverityAlarmLevel& state_variable_severity){
//    return AbstractControlUnit::getStateVariableSeverity(variable_type,
//                                                         state_variable_name,
//                                                         state_variable_severity);
//    
//}
//
//bool ProxyControlUnit::getStateVariableSeverity(chaos::cu::control_manager::StateVariableType variable_type,
//                                                const unsigned int state_variable_ordered_id,
//                                                common::alarm::MultiSeverityAlarmLevel& state_variable_severity){
//    return AbstractControlUnit::getStateVariableSeverity(variable_type,
//                                                         state_variable_ordered_id,
//                                                         state_variable_severity);
//}
//
//void ProxyControlUnit::setBusyFlag(bool state){
//    AbstractControlUnit::setBusyFlag(state);
//}
//
//const bool ProxyControlUnit::getBusyFlag() const {
//    return AbstractControlUnit::getBusyFlag();
//}
