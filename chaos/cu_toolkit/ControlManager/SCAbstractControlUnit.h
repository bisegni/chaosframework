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

#include <chaos/cu_toolkit/ControlManager/AbstractControlUnit.h>

#include <chaos/cu_toolkit/ControlManager/slow_command/SlowCommandExecutor.h>

namespace chaos_data = chaos::common::data;
namespace cs = chaos::cu::control_manager::slow_command;

namespace chaos {
    namespace cu {

        
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
            cu::control_manager::slow_command::SlowCommandExecutor *slowCommandExecutor;
            
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
			chaos_data::CDataWrapper* setDatasetAttribute(chaos_data::CDataWrapper *datasetAttributeValues, bool& detachParam) throw (CException);
            
			/*
             Event for update some CU configuration
             */
            chaos_data::CDataWrapper* updateConfiguration(chaos_data::CDataWrapper*, bool&) throw (CException);
        protected:
            
            virtual void defineSharedVariable();
            
            void addSharedVariable(std::string name, uint32_t max_size, chaos::DataType::DataType type);
            void setSharedVariableValue(std::string name, void *value, uint32_t value_size);
            // Get all managem declare action instance
            void _getDeclareActionInstance(std::vector<const DeclareAction *>& declareActionInstance);

        public:
            
            SCAbstractControlUnit();
            ~SCAbstractControlUnit();
            
            void setDefaultCommand(const char * dafaultCommandName);
            
            template<typename T>
            void installCommand(const char * commandName) {
                CHAOS_ASSERT(slowCommandExecutor)
                slowCommandExecutor->installCommand(std::string(commandName), SLOWCOMMAND_INSTANCER(T));
            }
        };
    }
}

#endif /* defined(__CHAOSFramework__SCAbstractControlUnit__) */
