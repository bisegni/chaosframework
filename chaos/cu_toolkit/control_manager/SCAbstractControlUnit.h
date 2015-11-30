/*
 *	SCAbstractControlUnit.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2013 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__SCAbstractControlUnit__
#define __CHAOSFramework__SCAbstractControlUnit__
#include <string>

#include <chaos/cu_toolkit/control_manager/AbstractControlUnit.h>

#include <chaos/cu_toolkit/control_manager/slow_command/SlowCommandExecutor.h>

namespace chaos {
    namespace cu {
		namespace control_manager {
			using namespace chaos::common::batch_command;
			using namespace chaos::common::data;
			using namespace chaos::cu::control_manager::slow_command;
			
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
				CDataWrapper* setDatasetAttribute(CDataWrapper *datasetAttributeValues, bool& detachParam) throw (CException);
				
				/*
				 Event for update some CU configuration
				 */
				CDataWrapper* updateConfiguration(CDataWrapper *update_pack, bool& detach_param) throw (CException);
			protected:
				//! Get all managem declare action instance
				void _getDeclareActionInstance(std::vector<const DeclareAction *>& declareActionInstance);
                
                //! called whr the infrastructure need to know how is composed the control unit
                void _defineActionAndDataset(CDataWrapper& setup_configuration)  throw(CException);
                
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
                                       CDataWrapper *slow_command_pack,
									   uint64_t& command_id);
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
				
				void setDefaultCommand(const std::string& dafaultCommandName, unsigned int sandbox_instance = 0);
				void addExecutionChannels(unsigned int execution_channels=1);
				
				template<typename T>
                void installCommand(const std::string& command_alias) {
					CHAOS_ASSERT(slow_command_executor)
					slow_command_executor->installCommand(command_alias, SLOWCOMMAND_INSTANCER(T));
				}
                
                void installCommand(boost::shared_ptr<BatchCommandDescription> command_description,
                                    bool is_default = false,
                                    unsigned int sandbox = 0);
			};
		}
    }
}

#endif /* defined(__CHAOSFramework__SCAbstractControlUnit__) */
