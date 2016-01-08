/*
 *	BatchCommandExecutor.h
 *	!CHAOS
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


#ifndef __CHAOSFramework__BatchCommandExecutor__
#define __CHAOSFramework__BatchCommandExecutor__

#include <map>
#include <deque>
#include <memory>
#include <vector>
#include <stdint.h>


#include <boost/thread.hpp>
#include <boost/heap/priority_queue.hpp>
#include <boost/atomic.hpp>

#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/action/DeclareAction.h>
#include <chaos/common/utility/ObjectInstancer.h>
#include <chaos/common/utility/StartableService.h>
#include <chaos/common/pqueue/CObjectProcessingPriorityQueue.h>
#include <chaos/common/batch_command/BatchCommandSandbox.h>
#include <chaos/common/batch_command/BatchCommandSandboxEventHandler.h>
#include <chaos/common/batch_command/BatchCommandDescription.h>
#include <chaos/common/thread/WaitSemaphore.h>
#include <chaos/common/async_central/async_central.h>
//#include <boost/container/deque.hpp>
//#include <boost/container/map.hpp>

#define COMMAND_QUEUE_DEFAULT_LENGTH		1024
#define COMMAND_STATE_QUEUE_DEFAULT_SIZE	50
#define COMMAND_BASE_SANDOXX_ID             0

namespace chaos_data = chaos::common::data;
namespace boost_cont = boost::container;

namespace chaos {
    namespace common {
        namespace batch_command {
            //forward declaration
            class BatchCommand;
            class BatchCommandSandbox;

            //! Macro for helping the allocation of the isntancer of the class implementing the slow command
            //#define BATCHCOMMAND_INSTANCER(SlowCommandClass) new chaos::common::utility::TypedObjectInstancer<SlowCommandClass, chaos::cu::control_manager::slow_command::BatchCommand>()

            typedef std::map<string,  boost::shared_ptr<BatchCommandDescription> >              MapCommandDescription;
            typedef std::map<string,  boost::shared_ptr<BatchCommandDescription> >::iterator    MapCommandDescriptionIterator;
            //! Slow command execution sand box
            /*!
             This class is the environment where the exeecution of the slow command handlers take place.
             */
            class BatchCommandExecutor:
            public utility::StartableService,
            public chaos::DeclareAction,
            public BatchCommandSandboxEventHandler,
            public chaos::common::async_central::TimerHandler {
                typedef boost::shared_mutex			RWMutex;
                typedef boost::shared_lock<RWMutex>	ReadLock;
                typedef boost::unique_lock<RWMutex>	WriteLock;


                //! executor identifier
                std::string executorID;

                boost::atomic_uint64_t command_sequence_id;

                //!point to the current executing command
                std::string     default_command_alias;
                unsigned int    default_command_sandbox_instance;

                //! Mutex for the map managment lock
                RWMutex sandbox_map_mutex;

                //! map for the sandbox instances
                std::map<unsigned int, boost::shared_ptr<BatchCommandSandbox> > sandbox_map;

                //! shared mutext foe the command event history
                RWMutex								command_state_rwmutex;
                //! command state queue dimension
                uint16_t							command_state_queue_max_size;

                //the queue of the insert state (this permit to have an order by insertion time)
                std::deque< boost::shared_ptr<CommandState> >			command_state_queue;
                //the map is used for fast access id/pointer
                std::map<uint64_t, boost::shared_ptr<CommandState> >	command_state_fast_access_map;

                //! this map correlate the alias to the object instancer
                MapCommandDescription map_command_description;


                //!Allocate a new slow command sandbox
                void addNewSandboxInstance();

                //! Add a new command state structure to the queue (checking the alredy presence)
                inline void addComamndState(uint64_t command_id);

                //! Thanke care to limit the size of the queue to the max size permitted
                inline void capCommanaQueue();

                //! Return a command stat
                boost::shared_ptr<CommandState> getCommandState(uint64_t command_sequence);
            protected:
                //command event handler
                virtual void handleCommandEvent(uint64_t command_seq, BatchCommandEventType::BatchCommandEventType type, void* type_value_ptr, uint32_t type_value_size);

                //! general sandbox event handler
                virtual void handleSandboxEvent(const std::string& sandbox_id, BatchSandboxEventType::BatchSandboxEventType type, void* type_value_ptr, uint32_t type_value_size);

                //! Global cache shared across the sandbox it can be alsog given
                AttributeValueSharedCache  global_attribute_cache;

                //! instanziate a command by CDataWrapper description
                /*!
                 Perform the instantiation of the command instace assocaite
                 to the alias contained into the submissionInfo param.
                 \param the submission param of the command
                 */
                virtual BatchCommand *instanceCommandInfo(const std::string& command_alias,
                                                          chaos_data::CDataWrapper *submissionInfo);

                //! instanziate a command by alias and submission parameter
                /*!
                 Perform the instantiation of the command instace assocaite
                 to the alias contained into the submissionInfo param.
                 \param the submission param of the command
                 */
                virtual BatchCommand *instanceCommandInfo(const std::string& command_alias,
                                                          uint32_t submission_rule = SubmissionRuleType::SUBMIT_NORMAL,
                                                          uint32_t submission_retry_delay = 1000,
                                                          uint64_t scheduler_step_delay = 1000000);

                //! Get the statistic for the current running command rpc action
                /*!
                 \ingroup API_Slow_Control
                 Return infromation about the execution statistic for the current running command, collected into the
                 chaos::cu::control_manager::slow_command::SandboxStat structure.
                 */
                chaos_data::CDataWrapper* getCommandState(chaos_data::CDataWrapper *params, bool& detachParam) throw (CException);


                //! Command features modification rpc action
                /*!
                 \ingroup API_Slow_Control
                 Updat ethe modiable features of the running command
                 */
                chaos_data::CDataWrapper* setCommandFeatures(chaos_data::CDataWrapper *params, bool& detachParam) throw (CException);

                //! Command features modification rpc action
                /*!
                 Updat ethe modiable features of the running command
                 */
                void setCommandFeatures(features::Features& features) throw (CException);

                //! Kill current command rpc action
                /*!
                 \ingroup API_Slow_Control
                 This METHOD perform the "brutal" remove of the current running command
                 from the scheduler. It waith the lock on the scehduler  and the provi to delete
                 the current command "as is".
                 */
                chaos_data::CDataWrapper* killCurrentCommand(chaos_data::CDataWrapper *params, bool& detachParam) throw (CException);

                //! Flush the command state history
                /*!
                 \ingroup API_Slow_Control
                 This METHOD perform the "flushing" of the command state queue (and hash). The flushing operation ensure that all,
                 non ended command state, will be remove from the history.
                 */
                chaos_data::CDataWrapper* flushCommandStates(chaos_data::CDataWrapper *params, bool& detachParam) throw (CException);

                //!Inherited by TimerHandler for capper operation
                void timeout();
            public:

                //! Private constructor
                BatchCommandExecutor(const std::string& _executorID);

                //! Private deconstructor
                virtual ~BatchCommandExecutor();

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
                 \param alias is the name of the command to use as default (started at startup)
                 \param sandbox_instance is the 1-based index of the sandbox where install the command
                 */
                void setDefaultCommand(const string& alias, unsigned int sandbox_instance = COMMAND_BASE_SANDOXX_ID);

                /*!
                 \ingroup API_Slow_Control
                 */
                const std::string & getDefaultCommand();

                //! return all the command description
                /*!
                 \ingroup API_Slow_Control
                 Fill the vector with all command description
                 */
                void getCommandsDescriptions(std::vector< boost::shared_ptr<BatchCommandDescription> >& descriptions);

                //! Install a command associated with a type
                /*!
                 Install the isntancer for a determinated SlowCommand, for an easly way to do this can be used
                 the macro SLOWCOMMAND_INSTANCER(SlowCommandClass) where "SlowCommandClass" is the calss that
                 extend SlowCommand to implement a new command. The access to the internal map is not sincornized
                 and need to be made befor the executor will be started
                 \param alias the name associated to the command
                 \param instancer the instance of the instancer that will produce the "instance" of the command
                 */
                void installCommand(const string& alias, chaos::common::utility::ObjectInstancer<BatchCommand> *instancer);

                //! Install a command by his description
                void installCommand(boost::shared_ptr<BatchCommandDescription> command_description);

                //!return all the aliases of the installe batch command
                /*!
                 \param commands_alias will be filled with the alias of the
                 registered commands
                 */
                void getAllCommandAlias(std::vector<std::string>& commands_alias);

                //! Submit a batch command
                /*!
                 The information for the command are contained into the DataWrapper data serialization,
                 they are put into the commandSubmittedQueue for to wait to be executed.
                 \param batch_command_alias alias of the batch command to submit
                 \param command_data the data of the command
                 \param command_id return the associated command id
                 */
                void submitCommand(const std::string& batch_command_alias,
                                   chaos_data::CDataWrapper *command_data,
                                   uint64_t& command_id)  throw (CException);

                //! Submit a batch command
                /*!
                 The information for the command are contained into the DataWrapper data serialization,
                 they are put into the commandSubmittedQueue for to wait to be executed.
                 \param batch_command_alias alias of the batch command to submit
                 \param command_data the data of the command
                 \param execution_channel is the index tha tidentify the sandbox where run the command
                 \param is the priority respect to other waiting instances
                 \param submission_rule, is the rule taht detarminate what appen to the current executing command,
                        when this instance nede to be executed within the sandbox
                 \param submission_retry_delay is the daly between a retry and another that the sandbox do for install command
                 \param scheduler_step_delay is delay between a run step and the next
                 \param command_id return the associated command id
                 */
                void submitCommand(const std::string& batch_command_alias,
                                   chaos_data::CDataWrapper *command_data,
                                   uint64_t& command_id,
                                   uint32_t execution_channel,
                                   uint32_t priority = 50,
                                   uint32_t submission_rule = SubmissionRuleType::SUBMIT_NORMAL,
                                   uint32_t submission_retry_delay = 1000,
                                   uint64_t scheduler_step_delay = 1000000)  throw (CException);

                //! return the state of a command
                std::auto_ptr<CommandState> getStateForCommandID(uint64_t command_id);

                //! Add a number of sandobx to this instance of executor
                void addSandboxInstance(unsigned int _sandbox_number);

                //! return the number of sandbox installed
                unsigned int getNumberOfSandboxInstance();

                void getSandboxID(std::vector<std::string> & sandbox_id);

                //! return the shared, between commadn, attribute cache
                AbstractSharedDomainCache *getAttributeSharedCache();
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__BatchCommandExecutor__) */
