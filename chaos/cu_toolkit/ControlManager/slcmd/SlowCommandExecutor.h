/*
 *	SlowCommandExecutor.h
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


#ifndef __CHAOSFramework__SlowCommandExecutor__
#define __CHAOSFramework__SlowCommandExecutor__

#include <memory>
#include <stdint.h>

#include <boost/thread.hpp>
#include <boost/lockfree/stack.hpp>
#include <boost/heap/priority_queue.hpp>

#include <chaos/common/utility/StartableService.h>
#include <chaos/common/pqueue/CObjectProcessingPriorityQueue.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/cu_toolkit/ControlManager/slcmd/SlowCommand.h>

#define COMMAND_QUEUE_DEFAULT_LENGTH 1024

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
        
        namespace cm {
            namespace slcmd {
                
                //forward declaration
                class SlowCommand;
                
                //! Slow command execution sand box
                /*!
                    This class is the environment where the exeecution of the slow command handlers take place.
                 */
                class SlowCommandExecutor : public utility::StartableService {
                    friend class AbstractControlUnit;
                    
                    AbstractControlUnit         *controUnitReference;
                    
                    shared_ptr<boost::thread>   incomingCheckThreadPtr;
                    
                    //!Shared muthex for write and read thread sinchronization
                    boost::mutex    queueMutext;
                    
                    //! Read thread waithing variable for the new insert element in queue
                    boost::condition_variable   readThreadWhait;
                    
                    //! Thread for whait until the queue is empty
                    boost::condition_variable   emptyQueueConditionLock;
                    
                    //! queue for the arrived command
                    boost::heap::priority_queue< PRIORITY_ELEMENT(CDataWrapper)* > commandSubmittedQueue;
                    
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
                  
                    //! Private constructor
                    SlowCommandExecutor(AbstractControlUnit *_controUnitReference);
                    
                    //! Private deconstructor
                    ~SlowCommandExecutor();
                    
                    //! Check the incoming command rule
                    /*!
                      In case there is a command in the top of the commandSubmittedQueue this method
                      check if there are the condition to execute it
                     */
                    void performIncomingCommandCheck();
                    
                public:
                    
                    
                    // Initialize instance
                    virtual void init(void*) throw(chaos::CException);
                    
                    
                    // Start the implementation
                    virtual void start() throw(chaos::CException);
                    
                    // Start the implementation
                    virtual void stop() throw(chaos::CException);
                    
                    
                    // Deinit the implementation
                    virtual void deinit() throw(chaos::CException);

                    
                    //! Perform a command registration
                    /*!
                     An instance of the command si registered within the executor.
                     */
                    void setDefaultCommand(const char *defaultCommandAlias);
                    
                    //! Submite the new sloc command information
                    /*!
                     The information for the command are contained into the DataWrapper data serialization,
                     they are put into the commandSubmittedQueue for to wait to be executed.
                     */
                    bool submitCommandInfo(CDataWrapper *commandDescription, uint8_t priority = 50, bool disposeOnDestroy = true);
                };
                
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__SlowCommandExecutor__) */
