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

namespace chaos {
    namespace cu {
        
        namespace cs = control_manager::slow_command;
        
        class ControManager;
        
        class SCAbstractControlUnit : public AbstractControlUnit {
            cu::control_manager::slow_command::SlowCommandExecutor *slowCommandExecutor;

        protected:
            friend class ControlManager;
            friend class DomainActionsScheduler;

            /*!
             Initialize the Custom Contro Unit and return the configuration
             */
            void init() throw(CException);
            
            /*!
             Internal implementation of the runmethod, that (for now) will schedule the slowcommand sandbox
             */
            void start() throw(CException);
            
            /*!
             Execute the Control Unit work
             */
            void stop() throw(CException);
            
            /*! 
             Deinit the Control Unit
             */
            void deinit() throw(CException);
            
            /*
             Receive the event for set the dataset input element, this virtual method
             is empty because can be used by controlunit implementation
             */
            CDataWrapper* setDatasetAttribute(CDataWrapper *datasetAttributeValues, bool& detachParam) throw (CException);
            
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
