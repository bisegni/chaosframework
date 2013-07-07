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

#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/pqueue/CObjectProcessingPriorityQueue.h>
#include <chaos/common/utility/InizializableService.h>

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
                
                class SlowCommandSandbox : public utility::InizializableService {
                    friend class AbstractControlUnit;
                    friend class SlowCommandExecutor;
                    
                    //!Mutex used for sincronize the introspection of the current command
                    boost::recursive_mutex  mutextCommandScheduler;
                    
                    CommandInfoAndImplementation *nextAvailableCommand;
                    
                    //! the stack for the command, The top commans is the one in execution
                    boost::lockfree::stack<SlowCommand*, boost::lockfree::fixed_sized<false> > commandStack;
                    
                    //-------------------- handler poiter --------------------
                    //! Pointer to the set phase handler's of the current command
                    SlowCommand::SetHandlerPtr setHandler;
                    
                    //! Pointer to the acquire pahse handler's of the current command
                    SlowCommand::AcquireHandlerPtr acquireHandler;
                    
                    //! Pointer to the correlation and commit pahse handler's of the current command
                    SlowCommand::CCHandlerPtr correlationAndCommitHandler;
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
                    inline bool trySetupAvailableCommand();
                    
                    //! Check the current command runnin state's and perform the necessary action
                    /*!
                     Perform the check of the running state of the current running command and perform
                     these task:
                     - remove running command according to the state
                     - pause the running command according to the state
                     - install the new available command according to the running state of the current
                     */
                    inline void checkRunningCommand();
                    
                protected:
                    
                    //! Initialize instance
                    void init(void*) throw(chaos::CException);
                    
                    //! Deinit the implementation
                    void deinit() throw(chaos::CException);
                    
                    //! Make a schedule cicle (acquire phase -> correlation phase -> check new command)
                    void runCommand();

                    void setNextAvailableCommand(PRIORITY_ELEMENT(CDataWrapper) *cmdInfo, SlowCommand *cmdImpl);
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__SlowCommandSandbox__) */
