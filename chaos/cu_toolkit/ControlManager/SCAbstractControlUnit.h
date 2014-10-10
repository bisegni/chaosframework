/*
 *	SCAbstractControlUnit.h
 *	!CHOAS
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

#include <chaos/cu_toolkit/ControlManager/AbstractControlUnit.h>

#include <chaos/cu_toolkit/ControlManager/slow_command/SlowCommandExecutor.h>

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
				
				/*
				 Receive the event for set the dataset input element, this virtual method
				 is empty because can be used by controlunit implementation
				 */
				CDataWrapper* setDatasetAttribute(CDataWrapper *datasetAttributeValues, bool& detachParam) throw (CException);
				
				/*
				 Event for update some CU configuration
				 */
				CDataWrapper* updateConfiguration(CDataWrapper*, bool&) throw (CException);
			protected:
				// Get all managem declare action instance
				void _getDeclareActionInstance(std::vector<const DeclareAction *>& declareActionInstance);
				
				//! system dataset configuraiton overload
				void initSystemAttributeOnSharedAttributeCache();
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
				
				void setDefaultCommand(std::string dafaultCommandName, unsigned int sandbox_instance = 0);
				void addExecutionChannels(unsigned int execution_channels=1);
				
				template<typename T>
				void installCommand(const char * commandName) {
					CHAOS_ASSERT(slow_command_executor)
					slow_command_executor->installCommand(std::string(commandName), SLOWCOMMAND_INSTANCER(T));
				}
			};
		}
    }
}

#endif /* defined(__CHAOSFramework__SCAbstractControlUnit__) */
