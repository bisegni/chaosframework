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
#include <chaos/common/batch_command/BatchCommandParallelSandbox.h>
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
            
            
            CHAOS_DEFINE_VECTOR_FOR_TYPE(ChaosSharedPtr<BatchCommandDescription>, BatchCommandDescriptionList);
            CHAOS_DEFINE_MAP_FOR_TYPE(std::string,  ChaosSharedPtr<BatchCommandDescription>, MapCommandDescription);
            
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
                //describe the type of sandbox to use
                bool serialized_sandbox;
                //! executor identifier
                std::string executorID;
                
                //!point to the current executing command
                std::string     default_command_alias;
                bool            default_command_stickyness;
                unsigned int    default_command_sandbox_instance;
                
                //! Mutex for the map managment lock
                RWMutex sandbox_map_mutex;
                
                //! map for the sandbox instances
                std::map<unsigned int, ChaosSharedPtr<AbstractSandbox> > sandbox_map;
                
                //! shared mutext foe the command event history
                RWMutex								command_state_rwmutex;
                
                //! command state queue dimension
                uint16_t							command_state_queue_max_size;
                
                //the queue of the insert state (this permit to have an order by insertion time)
                std::deque< ChaosSharedPtr<CommandState> >			command_state_queue;
                //the map is used for fast access id/pointer
                std::map<uint64_t, ChaosSharedPtr<CommandState> >	command_state_fast_access_map;
                
                //! this map correlate the alias to the object instancer
                MapCommandDescription map_command_description;
                
                
                //!Allocate a new slow command sandbox
                void addNewSandboxInstance();
                
                //! Add a new command state structure to the queue (checking the alredy presence)
                inline void addComamndState(uint64_t command_id);
                
                //! Thanke care to limit the size of the queue to the max size permitted
                inline void capCommanaQueue();
                
                //! Return a command stat
                ChaosSharedPtr<CommandState> getCommandState(uint64_t command_sequence);
                
            protected:
                //command event handler
                virtual void handleCommandEvent(uint64_t command_seq,
                                                BatchCommandEventType::BatchCommandEventType type,
                                                chaos::common::data::CDataWrapper *command_info,
                                                const BatchCommandStat& commands_stats);
                
                //! general sandbox event handler
                virtual void handleSandboxEvent(const std::string& sandbox_id,
                                                BatchSandboxEventType::BatchSandboxEventType type,
                                                void* type_value_ptr,
                                                uint32_t type_value_size);
                
                //! Global cache shared across the sandbox it can be alsog given
                chaos::common::data::cache::AttributeValueSharedCache  global_attribute_cache;
                
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
                chaos::common::data::CDWUniquePtr getCommandState(chaos::common::data::CDWUniquePtr params) throw (CException);
                
                
                //! Command features modification rpc action
                /*!
                 \ingroup API_Slow_Control
                 Updat ethe modiable features of the running command
                 */
                chaos::common::data::CDWUniquePtr setCommandFeatures(chaos::common::data::CDWUniquePtr params) throw (CException);
                
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
                chaos::common::data::CDWUniquePtr killCurrentCommand(chaos::common::data::CDWUniquePtr params) throw (CException);
                
                //! remove all pendig command form queue
                /*!
                 \ingroup API_Slow_Control
                 This METHOD perform the clean operation of the queue al the pending command will be deleted and not
                 will be executed.
                 */
                chaos::common::data::CDWUniquePtr clearCommandQueue(chaos::common::data::CDWUniquePtr params) throw (CException);
                
                //! Flush the command state history
                /*!
                 \ingroup API_Slow_Control
                 This METHOD perform the "flushing" of the command state queue (and hash). The flushing operation ensure that all,
                 non ended command state, will be remove from the history.
                 */
                chaos::common::data::CDWUniquePtr flushCommandStates(chaos::common::data::CDWUniquePtr params) throw (CException);
                
                //!Inherited by TimerHandler for capper operation
                void timeout();
            public:
                
                //! Private constructor
                BatchCommandExecutor(const std::string& _executorID,
                                     bool serialized_sandbox = true);
                
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
                void setDefaultCommand(const std::string& alias,
                                       unsigned int sandbox_instance = COMMAND_BASE_SANDOXX_ID);
                
                /*!
                 \ingroup API_Slow_Control
                 */
                const std::string & getDefaultCommand();
                
                //! return all the command description
                /*!
                 \ingroup API_Slow_Control
                 Fill the vector with all command description
                 */
                void getCommandsDescriptions(BatchCommandDescriptionList& descriptions);
                
                //! Install a command associated with a type
                /*!
                 Install the isntancer for a determinated SlowCommand, for an easly way to do this can be used
                 the macro SLOWCOMMAND_INSTANCER(SlowCommandClass) where "SlowCommandClass" is the calss that
                 extend SlowCommand to implement a new command. The access to the internal map is not sincornized
                 and need to be made befor the executor will be started
                 \param alias the name associated to the command
                 \param instancer the instance of the instancer that will produce the "instance" of the command
                 */
                void installCommand(const std::string& alias, chaos::common::utility::ObjectInstancer<BatchCommand> *instancer);
                
                //! Install a command by his description
                void installCommand(ChaosSharedPtr<BatchCommandDescription> command_description);
                
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
                 \param priority is the priority respect to other waiting instances
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
                ChaosUniquePtr<CommandState> getStateForCommandID(uint64_t command_id);
                
                //! Add a number of sandobx to this instance of executor
                void addSandboxInstance(unsigned int _sandbox_number);
                
                //! return the number of sandbox installed
                unsigned int getNumberOfSandboxInstance();
                
                void getSandboxID(std::vector<std::string> & sandbox_id);
                
                //! return the shared, between commadn, attribute cache
                chaos::common::data::cache::AbstractSharedDomainCache *getAttributeSharedCache();
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__BatchCommandExecutor__) */
