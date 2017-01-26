/*
 *	ControlUnitApiInterface.h
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

#ifndef __CHAOSFramework_B6CECC9A_9B2E_4842_BF61_2141D24A1C80_ControlUnitApiInterface_h
#define __CHAOSFramework_B6CECC9A_9B2E_4842_BF61_2141D24A1C80_ControlUnitApiInterface_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/data/DatasetDB.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/data/CDataVariant.h>
#include <chaos/common/alarm/MultiSeverityAlarm.h>

#include <chaos/common/utility/LockableObject.h>

#include <chaos/cu_toolkit/control_manager/AttributeSharedCacheWrapper.h>
#include <chaos/cu_toolkit/control_manager/ControlUnitTypes.h>

#include <boost/function.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>
namespace chaos {
    namespace cu {
        namespace control_manager {
            class ControlManager;
            class AbstractControlUnit;
            class ProxyControlUnit;

            
            CHAOS_DEFINE_LOCKABLE_OBJECT(AbstractControlUnit*, LKControlUnitInstancePtr)
            
            //! public interface for all control unit api needed by user
            class ControlUnitApiInterface {
                friend class ProxyControlUnit;
                friend class ControlManager;
                
                std::string dummy_str;
                mutable LKControlUnitInstancePtr control_unit_pointer;
                
                AttributeHandlerFunctor attribute_handler_functor;
                EventHandlerFunctor event_handler;
            protected:
                ControlUnitApiInterface();
                
                bool _variantHandler(const std::string& attribute_name,
                                     const chaos::common::data::CDataVariant& value);
                
                void fireEvent(const ControlUnitProxyEvent event_type);
            public:
                
                virtual ~ControlUnitApiInterface();
                
                //! Return the control unit instance
                const std::string& getCUID();
                
                //! get control unit load parameter
                const std::string& getCUParam();
                
                //! return the type of the control unit
                const std::string& getCUType();
                
                void setEventHandlerFunctor(EventHandlerFunctor _functor);
                
                //! Add dataset attribute
                /*!
                 Add the new attribute to the deviceID dataset specifing
                 the default parameter
                 \ingroup Control_Unit_User_Api
                 \ingroup Control_Unit_Definition_Api
                 \param attributeName the name of the new attribute
                 \param attributeDescription the description of the attribute
                 \param attributeType the type of the new attribute
                 \param attributeDirection the direction of the new attribute
                 */
                void addAttributeToDataSet(const std::string& attribute_name,
                                           const std::string& attribute_description,
                                           DataType::DataType attribute_type,
                                           DataType::DataSetAttributeIOAttribute attribute_direction,
                                           uint32_t maxSize = 0);
                /*!
                 \ingroup Control_Unit_User_Api
                 \ingroup Control_Unit_Definition_Api
                 */
                void addBinaryAttributeAsSubtypeToDataSet(const std::string& attribute_name,
                                                          const std::string& attribute_description,
                                                          DataType::BinarySubtype               subtype,
                                                          int32_t    cardinality,
                                                          DataType::DataSetAttributeIOAttribute attribute_direction);
                
                /*!
                 \ingroup Control_Unit_User_Api
                 \ingroup Control_Unit_Definition_Api
                 */
                void addBinaryAttributeAsSubtypeToDataSet(const std::string&            attribute_name,
                                                          const std::string&            attribute_description,
                                                          const std::vector<int32_t>&   subtype_list,
                                                          int32_t                       cardinality,
                                                          DataType::DataSetAttributeIOAttribute attribute_direction);
                
                /*!
                 \ingroup Control_Unit_User_Api
                 \ingroup Control_Unit_Definition_Api
                 */
                void addBinaryAttributeAsMIMETypeToDataSet(const std::string& attribute_name,
                                                           const std::string& attribute_description,
                                                           const std::string& mime_type,
                                                           DataType::DataSetAttributeIOAttribute attribute_direction);
                
                //!Get dataset attribute names
                /*!
                 Return all dataset attribute name
                 \ingroup Control_Unit_User_Api
                 \param attributesName the array that will be filled with the name
                 */
                void getDatasetAttributesName(ChaosStringVector& attributesName);
                
                //!Get device attribute name that has a specified direction
                /*!
                 Return all dataset attribute name
                 \ingroup Control_Unit_User_Api
                 \param directionType the direction for attribute filtering
                 \param attributesName the array that will be filled with the name
                 */
                void getDatasetAttributesName(DataType::DataSetAttributeIOAttribute directionType,
                                              ChaosStringVector& attributesName);
                
                //!Get  attribute description
                /*!
                 Return the dataset description
                 \ingroup Control_Unit_User_Api
                 \param attributesName the name of the attribute
                 \param attributeDescription the returned description
                 */
                void getAttributeDescription(const std::string& attributesName,
                                             std::string& attributeDescription);
                
                //!Get the value information for a specified attribute name
                /*!
                 Return the range value for the attribute
                 \ingroup Control_Unit_User_Api
                 \param attributesName the name of the attribute
                 \param rangeInfo the range and default value of the attribute
                 */
                int getAttributeRangeValueInfo(const std::string& attribute_name,
                                               chaos::common::data::RangeValueInfo& range_info);
                
                //!Set the range values for an attribute of the device
                /*!
                 set the range value for the attribute of the device
                 \ingroup Control_Unit_User_Api
                 \param attributesName the name of the attribute
                 \param rangeInfo the range and default value of the attribute, the fields
                 of the struct are not cleaned, so if an attrbute doesn't has
                 some finromation, relative field are not touched.
                 */
                void setAttributeRangeValueInfo(const std::string& attribute_name,
                                                chaos::common::data::RangeValueInfo& range_info);
                
                //!Get the direction of an attribute
                /*!
                 Return the direcion of the attribute
                 \ingroup Control_Unit_User_Api
                 \param attributesName the name of the attribute
                 \param directionType the direction of the attribute
                 */
                int getAttributeDirection(const std::string& attribute_name,
                                          DataType::DataSetAttributeIOAttribute& directionType);
                
                //---------------alarm api-------------
                //!create a new alarm into the catalog
                void addStateVariable(StateVariableType variable_type,
                                      const std::string& state_variable_name,
                                      const std::string& state_variable_description);
                
                //!set the severity on all state_variable
                void setStateVariableSeverity(StateVariableType variable_type,
                                              const chaos::common::alarm::MultiSeverityAlarmLevel state_variable_severity);
                
                //!set the state_variable state
                bool setStateVariableSeverity(StateVariableType variable_type,
                                              const std::string& state_variable_name,
                                              const chaos::common::alarm::MultiSeverityAlarmLevel state_variable_severity);
                //!set the StateVariable state
                bool setStateVariableSeverity(StateVariableType variable_type,
                                              const unsigned int state_variable_ordered_id,
                                              const chaos::common::alarm::MultiSeverityAlarmLevel state_variable_severity);
                //!get the current StateVariable state
                bool getStateVariableSeverity(StateVariableType variable_type,
                                              const std::string& state_variable_name,
                                              chaos::common::alarm::MultiSeverityAlarmLevel& state_variable_severity);
                //!get the current StateVariable state
                bool getStateVariableSeverity(StateVariableType variable_type,
                                              const unsigned int state_variable_ordered_id,
                                              chaos::common::alarm::MultiSeverityAlarmLevel& state_variable_severity);
                
                //! set the value on the busy flag
                void setBusyFlag(bool state);
                
                //!return the current value of the busi flag
                const bool getBusyFlag() const;
                
                AttributeSharedCacheWrapper * const getAttributeCache();
                
                //---------------------------handler function------------------------------
                void setAttributeHandlerFunctor(AttributeHandlerFunctor _functor);
                
                bool enableHandlerOnInputAttributeName(const std::string& attribute_name);
                
                bool removeHandlerOnInputAttributeName(const std::string& attribute_name);
                
                //---------------------------handler function------------------------------
                void pushOutputDataset();
                void pushInputDataset();
            };
        }
    }
}

#endif /* __CHAOSFramework_B6CECC9A_9B2E_4842_BF61_2141D24A1C80_ControlUnitApiInterface_h */
