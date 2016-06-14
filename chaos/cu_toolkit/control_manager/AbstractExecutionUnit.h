/*
 *	AbstractExecutionUnit.h
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

#ifndef __CHAOSFramework__AbstractExecutionUnit_h
#define __CHAOSFramework__AbstractExecutionUnit_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/io/IODataDriver.h>
#include <chaos/common/data/CDataVariant.h>
#include <chaos/common/data/cache/AttributeCache.h>
#include <chaos/common/message/MDSMessageChannel.h>
#include <chaos/common/message/DeviceMessageChannel.h>

#include <chaos/cu_toolkit/control_manager/RTAbstractControlUnit.h>

namespace chaos{
    namespace cu {
        namespace control_manager {
            
            //!  Base class for execution unit !CHAOS node.
            /*!
             This is the abstraction of the execution unit. It permit to run algorithm that can
             decalre other node where fetch data and other node to control. EU can also have a
             dataset to publish variable(output dataset) or receive dinamic ocnfiguration(input dataset)
             */
            class AbstractExecutionUnit:
            public RTAbstractControlUnit  {
                friend class ControlManager;
                friend class DomainActionsScheduler;
                //!is the alias associated to the algorithm
                std::string eu_description;
                
                //! keep track of last ts for step
                uint64_t last_execution_ts;
                
                //!inherited method
                void _defineActionAndDataset(chaos::common::data::CDataWrapper& setup_configuration) throw(CException);
                
                //! inherited method
                void unitRun() throw(CException);
                
                //! inherited method
                void unitInit() throw(CException);
                
                //! inherited method
                void unitStart() throw(CException);
                
                //! inherited method
                void unitStop() throw(CException);
                
                //! inherited method
                void unitDeinit() throw(CException);
                
            protected:
                
                /*!
                 Describe the functionality of the execution unit
                 \ingroup Execution_Unit_User_Api
                 \param description is the full description of the control unit
                 */
                void setExecutionUnitDescription(const std::string& description);
                
                //! add an attribute to the dataset fo the execution unit
                void addExecutionUnitAttributeToDataSet(const std::string& attribute_name,
                                                        const std::string& attribute_description,
                                                        DataType::DataType attribute_type,
                                                        DataType::DataSetAttributeIOAttribute attribute_direction,
                                                        uint32_t maxSize = 0);
                /*!
                 \ingroup Control_Unit_User_Api
                 \ingroup Control_Unit_Definition_Api
                 proxy for DatasetDB::addBinaryAttributeAsSubtypeToDataSet
                 */
                void addBinaryAttributeAsSubtypeToDataSet(const std::string& attribute_name,
                                                          const std::string& attribute_description,
                                                          DataType::BinarySubtype               subtype,
                                                          int32_t    cardinality,
                                                          DataType::DataSetAttributeIOAttribute attribute_direction);
                
                /*!
                 \ingroup Control_Unit_User_Api
                 \ingroup Control_Unit_Definition_Api
                 proxy for DatasetDB::addBinaryAttributeAsSubtypeToDataSet
                 */
                void addBinaryAttributeAsSubtypeToDataSet(const std::string&            attribute_name,
                                                          const std::string&            attribute_description,
                                                          const std::vector<int32_t>&   subtype_list,
                                                          int32_t                       cardinality,
                                                          DataType::DataSetAttributeIOAttribute attribute_direction);
                
                /*!
                 \ingroup Control_Unit_User_Api
                 \ingroup Control_Unit_Definition_Api
                 proxy for DatasetDB::addBinaryAttributeAsMIMETypeToDataSet
                 */
                void addBinaryAttributeAsMIMETypeToDataSet(const std::string& attribute_name,
                                                           const std::string& attribute_description,
                                                           std::string mime_type,
                                                           DataType::DataSetAttributeIOAttribute attribute_direction);
                
                //!Get the value from an output attribute
                /*!
                \ingroup Execution_Unit_User_Api
                \ingroup Execution_Unit_Definition_Api
                 */
                CDataVariant getOutputAttributeValue(const std::string& attribute_name);
                
                //!Get the value from an input attribute
                /*!
                 \ingroup Execution_Unit_User_Api
                 \ingroup Execution_Unit_Definition_Api
                 */
                CDataVariant getInputAttributeValue(const std::string& attribute_name);
                
                //!Set the value from an output attribute
                /*!
                 \ingroup Execution_Unit_User_Api
                 \ingroup Execution_Unit_Definition_Api
                 */
                void setOutputAttributeValue(const std::string& attribute_name,
                                             const CDataVariant& attribute_value);
                
                //!perform a step of the algorithm
                /*!
                 In this method the user can compute the spep of the algorithm to work on the newst data.
                 The sublcass need to implemente it to perform the algorithm
                 \ingroup Execution_Unit_User_Api
                 /param step_delay_time is the delay form the last step of the algorithm
                 */
                virtual void executeAlgorithmStep(uint64_t step_delay_time) throw (CException) = 0;
            public:
                
                /*! default constructor
                 \param _execution_unit_param is a string that contains parameter to pass during the contorl unit creation
                 \param _execution_unit_drivers driver information
                 */
                AbstractExecutionUnit(const std::string& _execution_unit_id,
                                      const std::string& _execution_unit_param);
                /*!
                 Parametrized constructor
                 \param _execution_unit_id unique id for the control unit
                 \param _execution_unit_param is a string that contains parameter to pass during the contorl unit creation
                 \param _execution_unit_drivers driver information
                 */
                AbstractExecutionUnit(const std::string& _execution_unit_id,
                                      const std::string& _execution_unit_param,
                                      const ControlUnitDriverList& _execution_unit_drivers);
                
                //!default destructor
                ~AbstractExecutionUnit();
            };
        }
    }
}

#endif /* __CHAOSFramework__AbstractExecutionUnit_h */
