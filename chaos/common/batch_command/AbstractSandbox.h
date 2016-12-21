/*
 *	AbstractSandbox.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 21/12/2016 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__0328897_245C_4675_98DE_F54215CB7934_AbstractSandbox_h
#define __CHAOSFramework__0328897_245C_4675_98DE_F54215CB7934_AbstractSandbox_h

#include <chaos/common/pqueue/CObjectProcessingPriorityQueue.h>
#include <chaos/common/batch_command/BatchCommand.h>
#include <chaos/common/batch_command/BatchCommandTypes.h>
#include <chaos/common/batch_command/BatchCommandSandboxEventHandler.h>
#include <chaos/common/utility/StartableService.h>

namespace chaos{
    namespace common {
        namespace batch_command {
            
            class BatchCommandExecutor;
            
            /*!
             Type used for the next available command impl and description
             into the sandbox
             */
            struct CommandInfoAndImplementation {
                CDataWrapper *cmdInfo;
                BatchCommand *cmdImpl;
                
                CommandInfoAndImplementation(CDataWrapper *_cmdInfo, BatchCommand *_cmdImpl);
                ~CommandInfoAndImplementation();
                
                void deleteInfo();
                void deleteImpl();
                
            };
            
            //!Base abstraction for sandbox implementation
            class AbstractSandbox:
            public utility::StartableService {
                friend class BatchCommandExecutor;
                friend struct AcquireFunctor;
                friend struct CorrelationFunctor;
            protected:
                //sandbox identification string
                std::string identification;
                
                //command sequence id
                boost::atomic_uint64_t command_sequence_id;
                
                
                //handler for sandbox event
                BatchCommandSandboxEventHandler *event_handler;
                
                void addCommandID(BatchCommand *command_impl);
            public:
                AbstractSandbox();
                
                virtual ~AbstractSandbox();
                
                virtual void killCurrentCommand() = 0;
                
                virtual void setCurrentCommandScheduerStepDelay(uint64_t scheduler_step_delay) = 0;
                
                virtual void lockCurrentCommandFeature(bool lock) = 0;
                
                virtual void setCurrentCommandFeatures(features::Features& features) throw (CException) = 0;
                
                virtual void setDefaultStickyCommand(BatchCommand *sticky_command) = 0;
                
                virtual bool enqueueCommand(chaos::common::data::CDataWrapper *command_to_info,
                                            BatchCommand *command_impl,
                                            uint32_t priority) = 0;
            };
        }
    }
}

#endif /* __CHAOSFramework__0328897_245C_4675_98DE_F54215CB7934_AbstractSandbox_h */
