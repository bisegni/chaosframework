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
#include <chaos/common/data/cache/AttributeCache.h>
#include <chaos/common/message/MDSMessageChannel.h>
#include <chaos/common/message/DeviceMessageChannel.h>

#include <chaos/cu_toolkit/control_manager/RTAbstractControlUnit.h>

namespace chaos{
    namespace cu {
        namespace control_manager {
                //!define an input or output variable
            /*
             this variable will be asociated, by configuration,
             to an input or output dataset. The configuraiton will
             be permitte by ccs or MDS API
             */
            struct VariableParameterDefinition {
                    //! is the alias associate to the variable
                std::string alias;

                    //! is the description that explaint the usage of the variable whitin the execution unit
                std::string description;

                    //! node class
                chaos::DataType::DataType type;

                    //! direction
                DataType::DataSetAttributeIOAttribute direction;

                    //! node is mandatory
                bool mandatory;

                    //! node dataset attribute that has been attacched to variable
                std::string node_dataset_attribute;

                    //! node unique that is used to fetch the dataset for variable
                std::string node_uid;

                VariableParameterDefinition():
                alias(),
                description(),
                type(DataType::TYPE_UNDEFINED),
                direction(DataType::Undefined),
                mandatory(false){}

                VariableParameterDefinition(const std::string& _alias,
                                            const std::string& _description,
                                            chaos::DataType::DataType _type,
                                            DataType::DataSetAttributeIOAttribute _direction,
                                            bool _mandatory):
                alias(_alias),
                description(_description),
                type(_type),
                direction(_direction),
                mandatory(_mandatory){}

                VariableParameterDefinition(const VariableParameterDefinition& source):
                alias(source.alias),
                description(source.description),
                type(source.type),
                direction(source.direction),
                mandatory(source.mandatory){}
            };

                //search on alias in the vector
            struct SearchVariableAlias {
                SearchVariableAlias(const std::string& _alias) : alias(_alias) {}
                bool operator()(const VariableParameterDefinition& obj) const
                {
                    return obj.alias.compare(alias) == 0;
                }
            private:
                const std::string& alias;
            };

                //!map that associate the name of variable to the description
            CHAOS_DEFINE_MAP_FOR_TYPE(std::string, VariableParameterDefinition, VPDMap);

                //!define map to correlate the node uid to control with respective device channel
            CHAOS_DEFINE_MAP_FOR_TYPE(std::string, chaos::common::message::DeviceMessageChannel*, DeviceMessageChannelMap);

                //!set for the variable aliases
            CHAOS_DEFINE_SET_FOR_TYPE(std::string, SetVariableAlias)

                //map to correlate the node uid with cached attribute (used for input variable)
            CHAOS_DEFINE_MAP_FOR_TYPE(std::string, SetVariableAlias, NodeUIDToVariableMap);

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

                    //!map that correlate the variable alias to the variable description
                VPDMap variable_map;

                    //!map that correlate the node uid to the associated input variable cache
                NodeUIDToVariableMap map_node_in_var_alias;

                    //!map that correlate the node uid to the associated ouput variable cache
                NodeUIDToVariableMap map_node_out_var_alias;

                    //! cache taht contains all variable cache
                boost::shared_ptr<chaos::common::data::cache::AttributeCache> input_variable_cache;

                    //! cache taht contains all variable cache
                boost::shared_ptr<chaos::common::data::cache::AttributeCache> output_variable_cache;

                    //!chaos mds channel for query metadata
                chaos::common::message::MDSMessageChannel *mds_msg_chnl;

                    //!high speed chaos query
                std::auto_ptr<chaos::common::io::IODataDriver> data_driver;

                    //!map for node uid->device message channel
                DeviceMessageChannelMap map_nuid_dmc;

                    //! keep track of last ts for step
                uint64_t last_execution_ts;

                    //!inherited method
                void _defineActionAndDataset(chaos::common::data::CDataWrapper& setup_configuration) throw(CException);

                    //! perform device message channel allcoation
                /*!
                 For all unique node uid associated to an output variable a device message channel
                 is allocated to perform the control of that device.
                 */
                void completeVaribleAndAllocateDMC(CDataWrapper& variable_configuration) throw(CException);

                    //!provide the deallcoation of all allocated device messag echannel
                void deallcoateDMC();

                    //! allcoate data io driver
                void initDataIODriver() throw(CException);

                    //! deinitialize the data io driver
                void deinitDataIODriver();

                    //! prepare the cache for the input and output variable
                void addVariableInCache(VariableParameterDefinition& input_variable);

                    //! inherited method
                void unitRun() throw(CException);

                    //! inherited method
                virtual void unitInit() throw(CException);

                    //! inherited method
                virtual void unitStart() throw(CException);

                    //! inherited method
                virtual void unitStop() throw(CException);

                    //! inherited method
                virtual void unitDeinit() throw(CException);

                    //! fetch all input variable values
                /*!
                 Scan map_node_var_alias maps with iterator in this way. For every node_uid,
                 the dataset is got from live and the associated set is scan for get the aliases
                 associated to it. For every alias is got the value form the dataset and updated
                 the cached one.
                 */
                void fetchInputVariableValues();

                    //! apply output variable modification
                /*!
                 get all output variable and fi there are change, it will be sent to the
                 corrispettive node.
                 */
                void applyModificationOnOutputNode();
            protected:

                /*!
                 Describe the functionality of the execution unit
                 \ingroup Execution_Unit_User_Api
                 \param description is the full description of the control unit
                 */
                void setExecutionUnitDescription(const std::string& description);

                /*!
                 Add a new execution unit variable specifying the property
                 \ingroup Execution_Unit_User_Api
                 \param alias is the name of the alias use for reference by execution unit
                 \param description
                 */
                void addExecutionUnitVariable(const std::string& alias,
                                              const std::string& description,
                                              chaos::DataType::DataType type,
                                              DataType::DataSetAttributeIOAttribute,
                                              bool mandatory);

                    //! return the pointer to the values for the input variable
                /*!
                 Access the variable cache and return directly the read only pointer to the cache buffer
                 \ingroup Execution_Unit_User_Api
                 */
                template<typename T>
                const T * getInputVariablePtr(const std::string& variable_name) {
                    AttributeValue *value_setting = input_variable_cache->getValueSettingByName(variable_name);
                    return (const T *)value_setting->getValuePtr<T>();
                }

                    //! set the value of and output variable
                /*!
                 Update the variable output value, after the algorithm step all output
                 variable values are sent to the respective node to update input attribute
                 of their dataset.
                 \ingroup Execution_Unit_User_Api
                 \param variable_name the name of the variable to update
                 \param value_ptr the pointer to the new value
                 \param the size of the new value
                 */
                void setOutputVariableValue(const std::string& variable_name,
                                            const void * value_ptr,
                                            uint32_t value_size);
                
                    //! set the value of and output variable
                /*!
                 Update the variable output value, after the algorithm step all output
                 variable values are sent to the respective node to update input attribute
                 of their dataset.
                 \ingroup Execution_Unit_User_Api
                 \param variable_name the name of the variable to update
                 \param value the value of the variable
                 */
                void setOutputVariableValue(const std::string& variable_name,
                                            bool value);

                    //! set the value of and output variable
                /*!
                 Update the variable output value, after the algorithm step all output
                 variable values are sent to the respective node to update input attribute
                 of their dataset.
                 \ingroup Execution_Unit_User_Api
                 \param variable_name the name of the variable to update
                 \param value the value of the variable
                 */
                void setOutputVariableValue(const std::string& variable_name,
                                            uint32_t value);

                    //! set the value of and output variable
                /*!
                 Update the variable output value, after the algorithm step all output
                 variable values are sent to the respective node to update input attribute
                 of their dataset.
                 \ingroup Execution_Unit_User_Api
                 \param variable_name the name of the variable to update
                 \param value the value of the variable
                 */
                void setOutputVariableValue(const std::string& variable_name,
                                            uint64_t value);

                    //! set the value of and output variable
                /*!
                 Update the variable output value, after the algorithm step all output
                 variable values are sent to the respective node to update input attribute
                 of their dataset.
                 \ingroup Execution_Unit_User_Api
                 \param variable_name the name of the variable to update
                 \param value the value of the variable
                 */
                void setOutputVariableValue(const std::string& variable_name,
                                            double value);

                    //! set the value of and output variable
                /*!
                 Update the variable output value, after the algorithm step all output
                 variable values are sent to the respective node to update input attribute
                 of their dataset.
                 \ingroup Execution_Unit_User_Api
                 \param variable_name the name of the variable to update
                 \param value the value of the variable
                 */
                void setOutputVariableValue(const std::string& variable_name,
                                            const std::string& value);

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
