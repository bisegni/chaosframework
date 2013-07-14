//
//  SlowCommandSandbox.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 7/8/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#ifndef __CHAOSFramework__SlowCommandSandbox__
#define __CHAOSFramework__SlowCommandSandbox__

#include <boost/thread.hpp>
#include <boost/lockfree/stack.hpp>
#include <boost/chrono.hpp>

#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/utility/StartableService.h>
#include <chaos/common/pqueue/CObjectProcessingPriorityQueue.h>

#include <chaos/cu_toolkit/DataManager/KeyDataStorage.h>
#include <chaos/cu_toolkit/ControlManager/slcmd/SlowCommand.h>

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
                
                typedef struct {
                    PRIORITY_ELEMENT(CDataWrapper) *cmdInfo;
                    SlowCommand *cmdImpl;
                } CommandInfoAndImplementation;

                //! Base functor for the command handler
                struct BaseFunctor {
                    SlowCommand *cmdInstance;
                };
                
                //! Functor implementation
                struct SetFunctor : public BaseFunctor {
                    uint8_t operator()(CDataWrapper *commandInfo) {
                        return cmdInstance?(cmdInstance->setHandler(commandInfo)):RunningStateType::RS_End;
                    }
                };
                
                struct AcquireFunctor : public BaseFunctor {
                    uint8_t operator()() {
                        return cmdInstance?(cmdInstance->acquireHandler()):RunningStateType::RS_End;
                    }
                };
                
                struct CorrelationFunctor : public BaseFunctor {
                    uint8_t operator()() {
                        return cmdInstance?(cmdInstance->ccHandler()):RunningStateType::RS_End;
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
                    boost::mutex  pauseMutex;
                    
                    //! Thread for whait until the queue is empty
                    boost::condition_variable  pauseCondition;
                    
                    boost::chrono::milliseconds checkTimeIntervall;
                    
                    boost::chrono::milliseconds schedulerStepDelay;

                    
                    //! POinte to the next available command
                    CommandInfoAndImplementation nextAvailableCommand;
                    
                    //point to the current executing command
                    SlowCommand *currentExecutingCommand;
                    
                    //! contain the paused command
                    boost::lockfree::stack<SlowCommand*, boost::lockfree::fixed_sized<false> > commandStack;
                    
                    //-------------------- handler poiter --------------------
                    //! Pointer to the set phase handler's of the current command
                    SetFunctor setHandlerFunctor;

                    //! Pointer to the acquire pahse handler's of the current command
                    AcquireFunctor acquireHandlerFunctor;
                    
                    //! Pointer to the correlation and commit pahse handler's of the current command
                    CorrelationFunctor correlationHandlerFunctor;
                    //-------------------- handler poiter --------------------
                                        
                    //! Check if the new command can be installed
                    /*!
                     Perform all check using the submission rule of the new command according
                     to the Running State of the current command. In case all goes weel the new
                     command is installed and the old one is managed accordint to the submissione rule
                     \param newCommandDescription the new command description that contain the submission rule
                     and the alias of the command to instantiate.
                     \return true if the command is successfull installed, false otherwise
                     */
                    inline void manageAvailableCommand();
                    
                    //!install the handler of the command
                    /*!
                     Perform th ecommand isntallation without check the condition. The handler
                     that are not implemented are managed according to the submition rule
                     */
                    inline void installHandler(SlowCommand *cmdImpl, CDataWrapper* setData);
                    
                    SlowCommandSandbox();
                    ~SlowCommandSandbox();
                    
                protected:
                    
                    //! Initialize instance
                    void init(void*) throw(chaos::CException);
                    
                    // Start the implementation
                    virtual void start() throw(chaos::CException);
                    
                    // Start the implementation
                    virtual void stop() throw(chaos::CException);
                    
                    //! Deinit the implementation
                    void deinit() throw(chaos::CException);
                    
                    //! Execute a compelte step of the command
                    /*!
                     Make a schedule cicle (acquire phase -> correlation phase -> check new command)
                     */
                    void runCommand();

                    bool setNextAvailableCommand(PRIORITY_ELEMENT(CDataWrapper) *cmdInfo, SlowCommand *cmdImpl);
                    
                    //set the dafault startup command
                    /*!
                     If the command can't be installed it isfree and an exception is fired
                     \param the SlowCommand implementation
                     */
                    void setDefaultCommand(SlowCommand *cmdImpl);
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__SlowCommandSandbox__) */
