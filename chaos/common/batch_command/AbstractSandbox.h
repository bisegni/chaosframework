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
            
#define SET_FAULT(l, c, m, d,f) \
SET_NAMED_FAULT(l, cmd_instance, c , m , d,f)
            
#define SET_NAMED_FAULT(l, n, c, m, d,f) \
l << c << m << d; \
n->setRunningProperty(f); \
n->fault_description.code = c; \
n->fault_description.description = m; \
n->fault_description.domain = d;
            
            //! Base functor for the command handler
            struct BaseFunctor {
                std::string sandbox_identifier;
                BatchCommand *cmd_instance;
            };
            
            //! Acquisition Functor implementation
            struct AcquireFunctor:
            public BaseFunctor {
                void operator()();
            };
            
            //! Correlation function implementation
            struct CorrelationFunctor:
            public BaseFunctor {
                void operator()();
            };
            

            struct EndFunctor:
                        public BaseFunctor {
                            void operator()();
                  };

            /*!
             Type used for the next available command impl and description
             into the sandbox
             */
            struct CommandInfoAndImplementation {
                chaos::common::data::CDataWrapper *cmdInfo;
                BatchCommand *cmdImpl;
                
                CommandInfoAndImplementation(chaos::common::data::CDataWrapper *_cmdInfo, BatchCommand *_cmdImpl);
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
