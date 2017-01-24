/*
 *	ProxyControlUnit.h
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

#ifndef __CHAOSFramework_EED208A4_A989_4DB4_9D1D_DB99F051738C_ProxyControlUnit_h
#define __CHAOSFramework_EED208A4_A989_4DB4_9D1D_DB99F051738C_ProxyControlUnit_h

#include <chaos/cu_toolkit/control_manager/ControlUnitApiInterface.h>
#include <chaos/cu_toolkit/control_manager/AbstractControlUnit.h>

namespace chaos {
    using namespace boost;
    using namespace boost::chrono;
    
    namespace cu {
        namespace control_manager {
            //forward declarations
            class ControManager;
            class AbstractExecutionUnit;
            
            class ProxyControlUnit:
            public cu::control_manager::ControlUnitApiInterface,
            public AbstractControlUnit {
                friend class ControlManager;
                friend class DomainActionsScheduler;
                friend class AbstractExecutionUnit;
                
                /*!
                 Define the control unit DataSet and Action into
                 a CDataWrapper
                 */
                void _defineActionAndDataset(chaos::common::data::CDataWrapper& setup_configuration) throw(CException);
                
                //! init rt control unit
                void init(void *initData) throw(CException);
                
                //! start rt control unit
                void start() throw(CException);
                
                //! stop rt control unit
                void stop() throw(CException);
                
                //! deinit rt control unit
                void deinit() throw(CException);

            protected:
                
                /*! default constructor
                 \param _control_unit_param is a string that contains parameter to pass during the contorl unit creation
                 \param _control_unit_drivers driver information
                 */
                ProxyControlUnit(const std::string& _alternate_type,
                                 const std::string& _control_unit_id,
                                 const std::string& _control_unit_param);
                /*!
                 Parametrized constructor
                 \param _control_unit_id unique id for the control unit
                 \param _control_unit_param is a string that contains parameter to pass during the contorl unit creation
                 \param _control_unit_drivers driver information
                 */
                ProxyControlUnit(const std::string& _alternate_type,
                                 const std::string& _control_unit_id,
                                 const std::string& _control_unit_param,
                                 const ControlUnitDriverList& _control_unit_drivers);
                
                /*! default constructor
                 \param _control_unit_param is a string that contains parameter to pass during the contorl unit creation
                 \param _control_unit_drivers driver information
                 */
                ProxyControlUnit(const std::string& _control_unit_id,
                                 const std::string& _control_unit_param);
                /*!
                 Parametrized constructor
                 \param _control_unit_id unique id for the control unit
                 \param _control_unit_param is a string that contains parameter to pass during the contorl unit creation
                 \param _control_unit_drivers driver information
                 */
                ProxyControlUnit(const std::string& _control_unit_id,
                                 const std::string& _control_unit_param,
                                 const ControlUnitDriverList& _control_unit_drivers);
                
                
                ~ProxyControlUnit();
                
                /*!
                 Event for update some CU configuration
                 */
                virtual chaos::common::data::CDataWrapper* updateConfiguration(CDataWrapper* update_pack,
                                                                               bool& detach_param) throw (CException);
                
                
            public:
//                //---------------definition api-------------
//                //! Add dataset attribute
//                /*!
//                 Add the new attribute to the deviceID dataset specifing
//                 the default parameter
//                 \ingroup Control_Unit_User_Api
//                 \ingroup Control_Unit_Definition_Api
//                 \param attributeName the name of the new attribute
//                 \param attributeDescription the description of the attribute
//                 \param attributeType the type of the new attribute
//                 \param attributeDirection the direction of the new attribute
//                 */
//                void addAttributeToDataSet(const std::string& attribute_name,
//                                           const std::string& attribute_description,
//                                           DataType::DataType attribute_type,
//                                           DataType::DataSetAttributeIOAttribute attribute_direction,
//                                           uint32_t maxSize = 0);
//                /*!
//                 \ingroup Control_Unit_User_Api
//                 \ingroup Control_Unit_Definition_Api
//                 */
//                void addBinaryAttributeAsSubtypeToDataSet(const std::string& attribute_name,
//                                                          const std::string& attribute_description,
//                                                          DataType::BinarySubtype               subtype,
//                                                          int32_t    cardinality,
//                                                          DataType::DataSetAttributeIOAttribute attribute_direction);
//                
//                /*!
//                 \ingroup Control_Unit_User_Api
//                 \ingroup Control_Unit_Definition_Api
//                 */
//                void addBinaryAttributeAsSubtypeToDataSet(const std::string&            attribute_name,
//                                                          const std::string&            attribute_description,
//                                                          const std::vector<int32_t>&   subtype_list,
//                                                          int32_t                       cardinality,
//                                                          DataType::DataSetAttributeIOAttribute attribute_direction);
//                
//                /*!
//                 \ingroup Control_Unit_User_Api
//                 \ingroup Control_Unit_Definition_Api
//                 */
//                void addBinaryAttributeAsMIMETypeToDataSet(const std::string& attribute_name,
//                                                           const std::string& attribute_description,
//                                                           std::string mime_type,
//                                                           DataType::DataSetAttributeIOAttribute attribute_direction);
//                
//                //!Get dataset attribute names
//                /*!
//                 Return all dataset attribute name
//                 \ingroup Control_Unit_User_Api
//                 \param attributesName the array that will be filled with the name
//                 */
//                void getDatasetAttributesName(vector<string>& attributesName);
//                
//                //!Get device attribute name that has a specified direction
//                /*!
//                 Return all dataset attribute name
//                 \ingroup Control_Unit_User_Api
//                 \param directionType the direction for attribute filtering
//                 \param attributesName the array that will be filled with the name
//                 */
//                void getDatasetAttributesName(DataType::DataSetAttributeIOAttribute directionType,
//                                              vector<string>& attributesName);
//                
//                //!Get  attribute description
//                /*!
//                 Return the dataset description
//                 \ingroup Control_Unit_User_Api
//                 \param attributesName the name of the attribute
//                 \param attributeDescription the returned description
//                 */
//                void getAttributeDescription(const string& attributesName,
//                                             string& attributeDescription);
//                
//                //!Get the value information for a specified attribute name
//                /*!
//                 Return the range value for the attribute
//                 \ingroup Control_Unit_User_Api
//                 \param attributesName the name of the attribute
//                 \param rangeInfo the range and default value of the attribute
//                 */
//                int getAttributeRangeValueInfo(const string& attributesName,
//                                               chaos_data::RangeValueInfo& rangeInfo);
//                
//                //!Set the range values for an attribute of the device
//                /*!
//                 set the range value for the attribute of the device
//                 \ingroup Control_Unit_User_Api
//                 \param attributesName the name of the attribute
//                 \param rangeInfo the range and default value of the attribute, the fields
//                 of the struct are not cleaned, so if an attrbute doesn't has
//                 some finromation, relative field are not touched.
//                 */
//                void setAttributeRangeValueInfo(const string& attributesName,
//                                                chaos_data::RangeValueInfo& rangeInfo);
//                
//                //!Get the direction of an attribute
//                /*!
//                 Return the direcion of the attribute
//                 \ingroup Control_Unit_User_Api
//                 \param attributesName the name of the attribute
//                 \param directionType the direction of the attribute
//                 */
//                int getAttributeDirection(const string& attributesName,
//                                          DataType::DataSetAttributeIOAttribute& directionType);
//                
//                //---------------alarm api-------------
//                //!create a new alarm into the catalog
//                void addStateVariable(chaos::cu::control_manager::StateVariableType variable_type,
//                                      const std::string& state_variable_name,
//                                      const std::string& state_variable_description);
//                
//                //!set the severity on all state_variable
//                void setStateVariableSeverity(chaos::cu::control_manager::StateVariableType variable_type,
//                                              const common::alarm::MultiSeverityAlarmLevel state_variable_severity);
//                
//                //!set the state_variable state
//                bool setStateVariableSeverity(chaos::cu::control_manager::StateVariableType variable_type,
//                                              const std::string& state_variable_name,
//                                              const common::alarm::MultiSeverityAlarmLevel state_variable_severity);
//                //!set the StateVariable state
//                bool setStateVariableSeverity(chaos::cu::control_manager::StateVariableType variable_type,
//                                              const unsigned int state_variable_ordered_id,
//                                              const common::alarm::MultiSeverityAlarmLevel state_variable_severity);
//                //!get the current StateVariable state
//                bool getStateVariableSeverity(chaos::cu::control_manager::StateVariableType variable_type,
//                                              const std::string& state_variable_name,
//                                              common::alarm::MultiSeverityAlarmLevel& state_variable_severity);
//                //!get the current StateVariable state
//                bool getStateVariableSeverity(chaos::cu::control_manager::StateVariableType variable_type,
//                                              const unsigned int state_variable_ordered_id,
//                                              common::alarm::MultiSeverityAlarmLevel& state_variable_severity);
//                
//                //! set the value on the busy flag
//                void setBusyFlag(bool state);
//                
//                //!return the current value of the busi flag
//                const bool getBusyFlag() const;
            };
        }
    }
}

#endif /* __CHAOSFramework_EED208A4_A989_4DB4_9D1D_DB99F051738C_ProxyControlUnit_h */
