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
#include <boost/heap/priority_queue.hpp>
#include <boost/atomic.hpp>

#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/data/CUSchemaDB.h>
#include <chaos/common/action/DeclareAction.h>
#include <chaos/common/utility/ObjectInstancer.h>
#include <chaos/common/utility/StartableService.h>
#include <chaos/common/pqueue/CObjectProcessingPriorityQueue.h>

#include <chaos/cu_toolkit/ControlManager/DeviceSchemaDB.h>
#include <chaos/cu_toolkit/ControlManager/slow_command/SlowCommandSandbox.h>
#include <chaos/cu_toolkit/ControlManager/slow_command/SlowCommandSandboxEventHandler.h>

#include <boost/container/deque.hpp>
#include <boost/container/map.hpp>

#define COMMAND_QUEUE_DEFAULT_LENGTH		1024
#define COMMAND_STATE_QUEUE_DEFAULT_SIZE	100

namespace chaos_data = chaos::common::data;
namespace boost_cont = boost::container;

namespace chaos {
    namespace cu {
        
        //forward declaration
        class SCAbstractControlUnit;
        
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
                class SlowCommandSandbox;
                
                //! Macro for helping the allocation of the isntancer of the class implementing the slow command
#define SLOWCOMMAND_INSTANCER(SlowCommandClass) new chaos::common::utility::TypedObjectInstancer<SlowCommandClass, chaos::cu::control_manager::slow_command::SlowCommand>()

				
                //! Slow command execution sand box
                /*!
                    This class is the environment where the exeecution of the slow command handlers take place.
                 */
                class SlowCommandExecutor : public utility::StartableService, public chaos::DeclareAction, public SlowCommandSandboxEventHandler {
					typedef boost::shared_mutex			RWMutex;
					typedef boost::shared_lock<RWMutex>	ReadLock;
					typedef boost::unique_lock<RWMutex>	WriteLock;
					
                    friend class chaos::cu::SCAbstractControlUnit;
                    
					//! executor identifier
                    std::string executorID;
                    
					boost::atomic_uint64_t command_sequence_id;
					
                    //!point to the current executing command
                    std::string defaultCommandAlias;
                    
                    //! sandbox for the execution of the handler
                    SlowCommandSandbox  commandSandbox;
                    
                    //! the reference to the master device database
                    DeviceSchemaDB *deviceSchemaDbPtr;
                    
                    boost::thread   *incomingCheckThreadPtr;
                    
                    //!Mutex for priority queue managment
                    boost::mutex    mutextQueueManagment;
                    
					//! shared mutext foe the command event history
					RWMutex								command_state_rwmutex;
					uint16_t							command_state_queue_max_size;
					//the queue of the insert state (this permit to have an order by insertion time)
					boost_cont::deque< boost::shared_ptr<CommandState> >			command_state_queue;
					//the map is used for fast access id/pointer
					boost_cont::map<uint64_t, boost::shared_ptr<CommandState> >		command_state_fast_access_map;
					
                    //! until it is true the wueue is
                    bool performQueueCheck;
                    
                    //! Read thread waithing variable for the new insert element in queue
                    boost::condition_variable   readThreadWhait;
                    
                    //! Thread for whait until the queue is empty
                    boost::condition_variable   emptyQueueConditionLock;
                    
                    //! queue for the arrived command
                    boost::heap::priority_queue< PRIORITY_ELEMENT(chaos_data::CDataWrapper)* > commandSubmittedQueue;

                    
                    //! this map correlate the alias to the object instancer
                    std::map<string, chaos::common::utility::ObjectInstancer<SlowCommand>* > mapCommandInstancer;
                    
                    //! Check the incoming command rule
                    /*!
                      In case there is a command in the top of the commandSubmittedQueue this method
                      check if there are the condition to execute it
                     */
                    void performIncomingCommandCheck();
                    
                    //! Check if the waithing command can be installed
                    /*!
						Perform the instantiation of the command instace assocaite
						to the alias contained into the submissionInfo param.
						\param the submission param of the command
                     */
                    SlowCommand *instanceCommandInfo(chaos_data::CDataWrapper *submissionInfo);
                    
                    //! Check if the waithing command can be installed
                    /*!
						peform the isntantiation of the command associated to the alias in input, if not preset
						an exception are fired
						\param commandAlias the alias of the command
                     */
                    SlowCommand *instanceCommandInfo(std::string& commandAlias);

					//command queue utility
					//! Add a new command state structure to the queue (checking the alredy presence)
					inline void addComamndState(uint64_t command_id);
					
					//! Thanke care to limit the size of the queue to the max size permitted
					inline void capCommanaQueue();
					
					//! Add a new command state structure to the queue (checking the alredy presence)
					inline boost::shared_ptr<CommandState> getCommandState(uint64_t command_sequence);
					
                protected:
                    
                    //! Private constructor
                    SlowCommandExecutor(std::string _executorID, DeviceSchemaDB *_deviceSchemaDbPtr);
                    
                    //! Private deconstructor
                    ~SlowCommandExecutor();
                    
					//command event handler
					void handleEvent(uint64_t command_seq, SlowCommandEventType::SlowCommandEventType type, void* type_attribute_ptr);
                    
                    //! Get queued command via rpc command
                    /*
						\ingroup API_Slow_Control
						Return the number and the infromation of the queued command via RPC
                     */
                    chaos_data::CDataWrapper* getQueuedCommand(chaos_data::CDataWrapper *params, bool& detachParam) throw (CException);
					
					//! Get the statistic for the current running command rpc action
                    /*!
						\ingroup API_Slow_Control
						Return infromation about the execution statistic for the current running command, collected into the 
						chaos::cu::control_manager::slow_command::SandboxStat structure.
                     */
					chaos_data::CDataWrapper* getCommandSandboxStatistics(chaos_data::CDataWrapper *params, bool& detachParam) throw (CException);
					
					
					//! Command features modification rpc action
                    /*!
						\ingroup API_Slow_Control
						Updat ethe modiable features of the running command
                     */
					chaos_data::CDataWrapper* setCommandFeatures(chaos_data::CDataWrapper *params, bool& detachParam) throw (CException);
					
					//! Kill current command rpc action
                    /*!
						\ingroup API_Slow_Control
						This METHOD perform the "brutal" remove of the current running command
						from the scheduler. It waith the lock on the scehduler  and the provi to delete 
						the current command "as is".
                     */
					chaos_data::CDataWrapper* killCurrentCommand(chaos_data::CDataWrapper *params, bool& detachParam) throw (CException);
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
                    void setDefaultCommand(string alias);
                    
                    //! Install a command associated with a type
                    /*!
                     Install the isntancer for a determinated SlowCommand, for an easly way to do this can be used
                     the macro SLOWCOMMAND_INSTANCER(SlowCommandClass) where "SlowCommandClass" is the calss that 
                     extend SlowCommand to implement a new command. The access to the internal map is not sincornized
                     and need to be made befor the executor will be started
                     \param alias the name associated to the command
                     \param instancer the instance of the instancer that will produce the "instance" of the command
                     */
                    void installCommand(string alias, chaos::common::utility::ObjectInstancer<SlowCommand> *instancer);
                    
                    //! Submite the new sloc command information
                    /*!
                     The information for the command are contained into the DataWrapper data serialization,
                     they are put into the commandSubmittedQueue for to wait to be executed.
                     */
                    bool submitCommand(chaos_data::CDataWrapper *commandDescription, uint64_t& command_id);
                    
                    //! set the custom data pointer of the channel setting instance to custom allocated memory
                    void setSharedCustomDataPtr(void *customDataPtr);
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__SlowCommandExecutor__) */
