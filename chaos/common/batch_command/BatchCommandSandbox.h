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
#include <chaos/common/exception/exception.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/data/cache/AttributeValueSharedCache.h>
#include <chaos/common/thread/WaitSemaphore.h>

#include <chaos/common/batch_command/AbstractSandbox.h>

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
                BatchCommand *cmd_instance;
            };
            
            //! Functor implementation
            struct AcquireFunctor : public BaseFunctor {
                void operator()();
            };
            
            struct CorrelationFunctor : public BaseFunctor {
                void operator()();
            };
            
            //! Sandbox fo the slow command execution
            /*!
             This is the sandbox where the command are executed. Here are checked the
             submission rule of the incoming command and the execution state of the current one.
             when they are compatible the current command will be killed or paused and new one
             come in.
             */
            class BatchCommandSandbox:
            public AbstractSandbox {
                friend class BatchCommandExecutor;
                friend struct AcquireFunctor;
                friend struct CorrelationFunctor;
                //stat for the single step of the command execution
                SandboxStat stat;
                BatchCommandStat cmd_stat;
                
                //-------shared data beetwen scheduler and checker thread------
                bool            schedule_work_flag;
                
                //! default sticky command
                std::auto_ptr<PRIORITY_ELEMENT(CommandInfoAndImplementation)>   default_sticky_command;
                
                //!point to the current executing command
                PRIORITY_ELEMENT(CommandInfoAndImplementation)   *current_executing_command;
                
                boost::mutex                    mutext_access_current_command;
                //boost::condition_variable_any   waithForNextCheck;
                WaitSemaphore					whait_for_next_check;
                
                //------------------scheduler---------------------
                //internal ascheduling thread
                std::auto_ptr<boost::thread> thread_scheduler;
                //! delay for the next beat of scheduler
                //uint64_t schedulerStepDelay;
                //! Thread for whait until the queue is empty
                WaitSemaphore  thread_scheduler_pause_condition;
                //bool   threadSchedulerPauseConditionFlag;
                //boost::condition_variable   threadSchedulerPauseCondition;
                
                //------------------next command checker---------------------
                //testing the inclusion of the command queue directly in the sandbox
                std::priority_queue<PRIORITY_ELEMENT(CommandInfoAndImplementation)*,
                std::vector<PRIORITY_ELEMENT(CommandInfoAndImplementation)*>,
                PriorityCommandCompare > command_submitted_queue;
                
                //!Mutex used for sincronize the introspection of the current command
                boost::mutex          mutex_next_command_queue;
                
                //! instance to the checker thread
                std::auto_ptr<boost::thread>    thread_next_command_checker;
                
                //! Thread for whait until the queue is empty
                boost::condition_variable_any  condition_waith_scheduler_end;
                
                //! Shared Channel Setting
                /*!
                 Shared aree to store the current and nex value
                 of the input channel or shared variable setting,
                 used into the control algoritm.
                 */
                AttributeValueSharedCache *shared_attribute_cache;
                
                //! contain the paused command
                std::stack<PRIORITY_ELEMENT(CommandInfoAndImplementation)*> command_stack;
                
                //-------------------- handler poiter --------------------
                //! Pointer to the acquire pahse handler's of the current command
                AcquireFunctor acquire_handler_functor;
                
                //! Pointer to the correlation and commit pahse handler's of the current command
                CorrelationFunctor correlation_handler_functor;
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
                
                //! check if there are command waiting to be submitted
                /*!
                 If are presento command to be submited, this method check the submission properting and the
                 curren executin gcommand running property. If there is a match new command is installed.
                 */
                void checkNextCommand();
                
                //!Lfat the fault information with the command information into a unique CDataWrapper
                inline std::auto_ptr<CDataWrapper> flatErrorInformationInCommandInfo(CDataWrapper *command_info,
                                                                                     FaultDescription& command_fault);
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
                
                //! enqueue a new command
                bool enqueueCommand(CDataWrapper *command_to_info, BatchCommand *command_impl, uint32_t priority);
                
                //! set the deafutl sticky command
                /*!
                 this comamnd is the one that is run when no other command
                 are presente neither into commadn queue or command stack
                 */
                void setDefaultStickyCommand(BatchCommand *command_impl);
            public:
                ~BatchCommandSandbox();
                //! Command features modification rpc action
                /*!
                 Updat ethe modiable features of the running command
                 */
                void setCurrentCommandFeatures(features::Features& features) throw (CException);
                
                void setCurrentCommandScheduerStepDelay(uint64_t scheduler_step_delay);
                
                void lockCurrentCommandFeature(bool lock);
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__BatchCommandSandbox__) */
