//
//  BatchCommandSandbox.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 7/8/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#ifndef __CHAOSFramework__BatchCommandSandbox__
#define __CHAOSFramework__BatchCommandSandbox__

#include <queue>
#include <string>
#include <stack>
#include <memory>

#include <boost/thread.hpp>
#include <boost/chrono.hpp>
//#include <boost/heap/priority_queue.hpp>
#include <chaos/common/exception/CException.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/data/cache/AttributesSetting.h>
#include <chaos/common/utility/StartableService.h>
#include <chaos/common/thread/WaitSemaphore.h>
#include <chaos/common/pqueue/CObjectProcessingPriorityQueue.h>

#include <chaos/common/batch_command/BatchCommand.h>
#include <chaos/common/batch_command/BatchCommandTypes.h>
#include <chaos/common/batch_command/BatchCommandSandboxEventHandler.h>

namespace chaos{
    namespace common {
        namespace batch_command {
			
			using namespace chaos::common::data;
			using namespace chaos::common::data::cache;
			
            //forward declaration
            class BatchCommand;
            class BatchCommandExecutor;
            
            //! Base functor for the command handler
            struct BaseFunctor {
                std::string sandbox_identifier;
                BatchCommand *cmdInstance;
            };
            
            //! Functor implementation
            struct AcquireFunctor : public BaseFunctor {
                void operator()();
            };
            
            struct CorrelationFunctor : public BaseFunctor {
                void operator()();
            };
            
            
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
            
            // pulic class used into the sandbox for use the priority set into the lement that are pointer and not rela reference
            struct PriorityCommandCompare {
                bool operator() (const PRIORITY_ELEMENT(CommandInfoAndImplementation)* lhs, const PRIORITY_ELEMENT(CommandInfoAndImplementation)* rhs) const {
                    return (lhs->priority < rhs->priority);
                }
            };
            
            //! SAndbox fo the slow command execution
            /*!
             This is the sandbox where the command are executed. Here are checked the
             submission rule of the incoming command and the execution state of the current one.
             when they are compatible the current command will be killed or paused and new one
             come in.
             */
            class BatchCommandSandbox : public chaos::utility::StartableService {
                friend class BatchCommandExecutor;
                friend struct AcquireFunctor;
                friend struct CorrelationFunctor;
                //stat for the single step of the command execution
                SandboxStat stat;
                
                //sandbox identification string
                std::string identification;
                
                //handler for sandbox event
                BatchCommandSandboxEventHandler *event_handler;
                
                //-------shared data beetwen scheduler and checker thread------
                bool            scheduleWorkFlag;
                
                //!point to the current executing command
                PRIORITY_ELEMENT(CommandInfoAndImplementation)   *currentExecutingCommand;
                
                boost::mutex                    mutextAccessCurrentCommand;
                //boost::condition_variable_any   waithForNextCheck;
                WaitSemaphore					waithForNextCheck;
                
                //------------------scheduler---------------------
                //internal ascheduling thread
                std::auto_ptr<boost::thread> threadScheduler;
                //! delay for the next beat of scheduler
                //uint64_t schedulerStepDelay;
                //! Thread for whait until the queue is empty
                WaitSemaphore  threadSchedulerPauseCondition;
                //bool   threadSchedulerPauseConditionFlag;
                //boost::condition_variable   threadSchedulerPauseCondition;
                
                //------------------next command checker---------------------
                //testing the inclusion of the command queue directly in the sandbox
                std::priority_queue<PRIORITY_ELEMENT(CommandInfoAndImplementation)*,
                std::vector<PRIORITY_ELEMENT(CommandInfoAndImplementation)*>,
                PriorityCommandCompare > command_submitted_queue;
                
                //!Mutex used for sincronize the introspection of the current command
                boost::recursive_mutex          mutexNextCommandChecker;
                
                //! instance to the checker thread
                std::auto_ptr<boost::thread>    threadNextCommandChecker;
                
                //! Thread for whait until the queue is empty
                boost::condition_variable_any  conditionWaithSchedulerEnd;
                
                //! Shared Channel Setting
                /*!
                 Shared aree to store the current and nex value
                 of the input channel or shared variable setting,
                 used into the control algoritm.
                 */
                AttributeValueSharedCache *shared_attribute_cache;
                
                //! contain the paused command
                std::stack<PRIORITY_ELEMENT(CommandInfoAndImplementation)*> commandStack;
                
                //-------------------- handler poiter --------------------
                //! Pointer to the acquire pahse handler's of the current command
                AcquireFunctor acquireHandlerFunctor;
                
                //! Pointer to the correlation and commit pahse handler's of the current command
                CorrelationFunctor correlationHandlerFunctor;
                //-------------------- handler poiter --------------------
                
                //!install the handler of the command
                /*!
                 Perform th ecommand isntallation without check the condition. The handler
                 that are not implemented are managed according to the submition rule
                 */
                inline bool installHandler(PRIORITY_ELEMENT(CommandInfoAndImplementation)* cmd_to_install);
				
				//! remove an handler fo a command
                inline void removeHandler(PRIORITY_ELEMENT(CommandInfoAndImplementation)* cmd_to_install);
				
				//kill the current running command without rule(like -9)
                void killCurrentCommand();
				
				//default private constructor and destructor
                BatchCommandSandbox();
                ~BatchCommandSandbox();
                
                //! execute a complete step of the command (acquire -> correlation) and check if the new command can be installed
                /*!
                 Perform all check using the submission rule of the new command according
                 to the Running State of the current command. In case all goes weel the new
                 command is installed and the old one is managed accordint to the submissione rule
                 \param newCommandDescription the new command description that contain the submission rule
                 and the alias of the command to instantiate.
                 \return true if the command is successfull installed, false otherwise
                 */
                void runCommand();
                
                void checkNextCommand();
                
            protected:
                
                void* sharedSettingPtr;
                
                //! Initialize instance
                void init(void*) throw(chaos::CException);
                
                // Start the implementation
                virtual void start() throw(chaos::CException);
                
                // Start the implementation
                virtual void stop() throw(chaos::CException);
                
                // Deinit the implementation
                void deinit() throw(chaos::CException);
                
                bool enqueueCommand(CDataWrapper *command_to_info, BatchCommand *command_impl, uint32_t priority);
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__BatchCommandSandbox__) */
