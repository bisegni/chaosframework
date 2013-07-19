//
//  SlowCommandSandbox.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 7/8/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#ifndef __CHAOSFramework__SlowCommandSandbox__
#define __CHAOSFramework__SlowCommandSandbox__

#include <stack>

#include <boost/thread.hpp>
#include <boost/chrono.hpp>

#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/utility/StartableService.h>
#include <chaos/common/thread/WaitSemaphore.h>
#include <chaos/common/pqueue/CObjectProcessingPriorityQueue.h>

#include <chaos/cu_toolkit/DataManager/KeyDataStorage.h>
#include <chaos/cu_toolkit/ControlManager/slcmd/SlowCommand.h>
#include <chaos/cu_toolkit/ControlManager/slcmd/ChannelSetting.h>
#include <chaos/cu_toolkit/ControlManager/slcmd/SlowCommandTypes.h>

namespace chaos{
    namespace cu {
        
        //forward declaration
        class AbstractControlUnit;
        
        //forward declaration
        namespace dm {
            namespace driver {
                class DriverAccessor;
            }
        }
        
                
        namespace control_manager {
            namespace slow_command {
                //forward declaration
                class SlowCommand;
                class SlowCommandExecutor;

                //! Base functor for the command handler
                struct BaseFunctor {
                    SlowCommand *cmdInstance;
                };
                
                //! Functor implementation
                struct SetFunctor : public BaseFunctor {
                    void operator()(CDataWrapper *commandInfo) {
                        if(cmdInstance) cmdInstance->setHandler(commandInfo);
                    }
                };
                
                struct AcquireFunctor : public BaseFunctor {
                    void operator()() {
                        if(cmdInstance) cmdInstance->acquireHandler();
                    }
                };
                
                struct CorrelationFunctor : public BaseFunctor {
                    void operator()() {
                        if(cmdInstance) (cmdInstance->ccHandler());
                    }
                };
                
                //! SAndbox fo the slow command execution
                /*!
                    This is the sandbox where the command are executed. Here are checked the
                    submission rule of the incoming command and the execution state of the current one.
                    when they are compatible the current command will be killed or paused and new one
                    come in.
                 */
                class SlowCommandSandbox : public utility::StartableService {
                    friend class chaos::cu::AbstractControlUnit;
                    friend class SlowCommandExecutor;
                    
                    
                    //internal ascheduling thread
                    boost::thread *schedulerThread;
                    
                    bool scheduleWorkFlag;
                    
                    
                    KeyDataStorage*  keyDataStorage;
                    
                    //!Mutex used for sincronize the introspection of the current command
                    boost::recursive_mutex  mutextCommandScheduler;
                    
                    //! Thread for whait until the queue is empty
                    boost::condition_variable_any  conditionWaithSchedulerEnd;
                    
                    //!Mutex used for sincronize the introspection of the current command
                    //boost::mutex  pauseMutex;
                    
                    //! Thread for whait until the queue is empty
                    WaitSemaphore  pauseCondition;
                    
                    //! check time intervall for the valutate the jobs (next, current and paused) state
                    boost::chrono::milliseconds checkTimeIntervall;
                    
                    //! delay for the next beat of scheduler
                    boost::chrono::milliseconds schedulerStepDelay;

                    //! Pointers to the next available command implementation and information
                    CommandInfoAndImplementation nextAvailableCommand;
                    
                    //!point to the current executing command
                    SlowCommand *currentExecutingCommand;
                    
                    //! Shared Channel Setting 
                    /*!
                        Shared aree to store the current and nex value
                        of the input channel or shared variable setting,
                        used into the control algoritm.
                     */
                    ChannelSetting sharedChannelSetting;
                    
                    //! contain the paused command
                    std::stack<SlowCommand*> commandStack;
                    
                    //-------------------- handler poiter --------------------
                    //! Pointer to the set phase handler's of the current command
                    SetFunctor setHandlerFunctor;

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
                    inline void installHandler(SlowCommand *cmdImpl, CDataWrapper* setData);
                    
                    inline SlowCommand* getStackTopOrWaithOnMutext(boost::recursive_mutex::scoped_lock& lockScheduler);
                    
                    SlowCommandSandbox();
                    ~SlowCommandSandbox();
                    
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

                    bool setNextAvailableCommand(PRIORITY_ELEMENT(CDataWrapper) *cmdInfo, SlowCommand *cmdImpl);
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__SlowCommandSandbox__) */
