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

#ifndef __CHAOSFramework__SCAbstractControlUnit__
#define __CHAOSFramework__SCAbstractControlUnit__

//#include <chaos/common/chaos_types.h>

#include <chaos/cu_toolkit/control_manager/AbstractControlUnit.h>

#include <chaos/cu_toolkit/control_manager/slow_command/SlowCommandExecutor.h>

namespace chaos {
    namespace cu {
        namespace control_manager {
            class ControManager;
            
            //! Abstract class for Slow Control Unit
            /*!
             This class is the base for the slow control unit. It manage the all
             customization of the base class AbstractControlUnit necessary to implement
             slow control. With this term is intended a command that when it is executed,
             it can run in a unterminated period of time.
             
             Subclass need to install commands and, optionally, set the default one.
             */
            class SCAbstractControlUnit : public AbstractControlUnit {
                friend class ControlManager;
                friend class DomainActionsScheduler;
                
                //! Slow command executor pointer
                cu::control_manager::slow_command::SlowCommandExecutor *slow_command_executor;
                
                // Startable Service method
                void init(void *initData) throw(CException);
                
                // Startable Service method
                void start() throw(CException);
                
                // Startable Service method
                void stop() throw(CException);
                
                // Startable Service method
                void deinit() throw(CException);
                
                //redefine private for protection
                AbstractSharedDomainCache *_getAttributeCache();
                
                /*
                 Receive the event for set the dataset input element, this virtual method
                 is empty because can be used by controlunit implementation
                 */
                chaos::common::data::CDataWrapper* setDatasetAttribute(chaos::common::data::CDataWrapper *dataset_attribute_values, bool& detachParam) throw (CException);
                
                //!inherited method by @AbstractControlUnit
                void propertyUpdatedHandler(const std::string& group_name,
                                            const std::string& property_name,
                                            const chaos::common::data::CDataVariant& old_value,
                                            const chaos::common::data::CDataVariant& new_value);
            protected:
                //! Get all managem declare action instance
                void _getDeclareActionInstance(std::vector<const DeclareAction *>& declareActionInstance);
                
                //! called whr the infrastructure need to know how is composed the control unit
                void _defineActionAndDataset(chaos::common::data::CDataWrapper& setup_configuration)  throw(CException);
                
                //! system dataset configuraiton overload
                void initSystemAttributeOnSharedAttributeCache();
                
                //! compelte th einput attribute with the alias of the slow commands registered
                void completeInputAttribute();
                
                //! push dataset
                /*!
                 \ingroup Control_Unit_User_Api
                 \param slow_command_pack is the whole command pack that contains either
                 the alias and the parameter (for engine e for execution) of the command.
                 \param command_id is the filed where is returned the unique id associated to the submitted command
                 */
                void submitSlowCommand(const std::string command_alias,
                                       chaos::common::data::CDataWrapper *slow_command_pack,
                                       uint64_t& command_id);
                
                //! Submit a batch command
                /*!
                 The information for the command are contained into the DataWrapper data serialization,
                 they are put into the commandSubmittedQueue for to wait to be executed.
                 \param batch_command_alias alias of the batch command to submit
                 \param command_data the data of the command
                 \param execution_channel is the index tha tidentify the sandbox where run the command
                 \param is the priority respect to other waiting instances
                 \param submission_rule, is the rule taht detarminate what appen to the current executing command,
                 when this instance nede to be executed within the sandbox
                 \param submission_retry_delay is the daly between a retry and another that the sandbox do for install command
                 \param scheduler_step_delay is delay between a run step and the next
                 \param command_id return the associated command id
                 */
                void submitBatchCommand(const std::string& batch_command_alias,
                                        chaos_data::CDataWrapper *command_data,
                                        uint64_t& command_id,
                                        uint32_t execution_channel,
                                        uint32_t priority = 50,
                                        uint32_t submission_rule = chaos::common::batch_command::SubmissionRuleType::SUBMIT_NORMAL,
                                        uint32_t submission_retry_delay = 1000,
                                        uint64_t scheduler_step_delay = 1000000)  throw (CException);
                
                //!return a command description for a determinate uid
                ChaosUniquePtr<chaos::common::batch_command::CommandState> getStateForCommandID(uint64_t command_id);
                
            public:
                
                /*! default constructor
                 \param _control_unit_id unique id for the control unit
                 \param _control_unit_param is a string that contains parameter to pass during the contorl unit creation
                 */
                SCAbstractControlUnit(const std::string& _control_unit_id,
                                      const std::string& _control_unit_param);
                /*!
                 Parametrized constructor
                 \param _control_unit_id unique id for the control unit
                 \param _control_unit_param is a string that contains parameter to pass during the contorl unit creation
                 \param _control_unit_drivers driver information
                 */
                SCAbstractControlUnit(const std::string& _control_unit_id,
                                      const std::string& _control_unit_param,
                                      const ControlUnitDriverList& _control_unit_drivers);
                
                ~SCAbstractControlUnit();
                
                void setDefaultCommand(const std::string& dafaultCommandName,
                                       bool sticky = true,
                                       unsigned int sandbox_instance = 0);
                void addExecutionChannels(unsigned int execution_channels=1);
                
                template<typename T>
                void installCommand(const std::string& command_alias) {
                    CHAOS_ASSERT(slow_command_executor)
                    slow_command_executor->installCommand(command_alias, SLOWCOMMAND_INSTANCER(T));
                }
                
                void installCommand(ChaosSharedPtr<chaos::common::batch_command::BatchCommandDescription> command_description,
                                    bool is_default = false,
                                    bool sticky = true,
                                    unsigned int sandbox = 0);
                
                bool waitOnCommandID(uint64_t& cmd_id);
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__SCAbstractControlUnit__) */
