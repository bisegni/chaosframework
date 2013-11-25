/*
 *	chaos_types.h
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

#ifndef CHAOSFramework_chaos_types_h
#define CHAOSFramework_chaos_types_h
namespace chaos {
    namespace cu {
        namespace control_manager {
            namespace slow_command {
                //! Namespace for the slow command event type within the framework
                namespace SlowCommandEventType {
					
					/*!
                     * \enum SlowCommandEventType
                     * \brief Describe the event that the sand box can forward for notify the current state of a command
                     */
					typedef enum SlowCommandEventType {
						EVT_QUEUED = 0,		/**< The command is queued */
						EVT_WAITING=1,		/**< The command is the next command to executed and i whaiting the end of the current */
						EVT_RUNNING=2,		/**< The command is running */
						EVT_PAUSED=3,			/**< The command is paused */
						EVT_COMPLETED=4,		/**< The command has completed is work successfully */
						EVT_FAULT=5,			/**< The command has fault */
						EVT_KILLED=6			/**< The event has been killed */
					} SlowCommandEventType;
				}
				
                /*!
                 \struct FaultDescription
                 \brief  Describe the fault of the command. This fileds need to be valorized
                 before to set the rState to RunningState::RS_Fault option
                 */
                typedef struct FaultDescription {
                    uint32_t    code;           /**< The number code of the error */
                    std::string description;    /**< The description of the fault */
                    std::string domain;         /**< The domain identify the context where the fault is occured */
                } FaultDescription;
                
				
				/*!
				 \struct CommandState
				 \brief  The structure used for querying the command state, the slowcommand scheduler
				 keep a queue for some numebr of command (also terminated) at every event @SlowCommandEventType::SlowCommandEventType
				 the state is updated
				 */
				struct CommandState {
					//! unique command identification number
					uint64_t command_id;
					
					//! last event occurend on the command
					SlowCommandEventType::SlowCommandEventType last_event;
					
					//! the fault description
					FaultDescription fault_description;
				};
				
                //! Namespace for the slow command submisison rule
                namespace SubmissionRuleType {
                    /*!
                     * \enum SubmissionRule
                     * \brief Describe the state in which the command can be found
                     */
                    typedef enum SubmissionRule {
                        SUBMIT_AND_Stack    = 0,    /**< The new command wil stack the current executing command that consist in
                                                     install all implemented handler of the new one without touch the handler that are not implemented */
                        SUBMIT_AND_Kill     = 1,    /**< The new command will kill the current command, all hadnler ol killed one are erased and substituted */
                        SUBMIT_NORMAL       = 2     /**< The new command will waith the end of the current executed command and if an handler is implemented it is installed*/
                    } SubmissionRule;
                }
				
				/*!
				 \struct SandboxStat
				 Used for collect the statistic about current running command.
                 */
                typedef struct {
                    uint64_t lastCmdStepStart;	/**< Represent the time collected at the start of the scehduler step (before acquisition phase) */
                    uint64_t lastCmdStepTime;	/**< Represent the time collected at the end of the scehduler step (befor the sleep or pause of the thread) */
                } SandboxStat;
				
				//! Namespace for the features of the slow command
				namespace features {
					
					//! Namespace for the features types of the slow command
					namespace FeaturesFlagTypes {
						/*!
						 * \enum FeatureFlag
						 * \brief The feature represent the custumoziation
						 * of the sandbox ad the moment the slowcommadn is installed
						 */
						typedef enum {
							FF_SET_SCHEDULER_DELAY      = 1,    /**< The command bring his own scheduler delay time */
							FF_SET_SUBMISSION_RETRY     = 2,    /**< The delay between a submiossion check and another, submiossion chek consist to check the current command running state to determinate
																 when the new command can be installed */
							FF_SET_COMMAND_TIMEOUT		= 4,    /**< The timeout of the current istnace of the command, expressed in microsecond */

							FF_LOCK_USER_MOD			= 8		/**< Active or deactivation of the user modificaiton on the command features */
						} FeatureFlag;
					}
					
						//! Features of the sloc control command
					typedef struct {
						//! Features flags
						/*!
						 One or more active flag from FeaturesFlagTypes::FeatureFlag
						 */
						uint8_t  featuresFlag;
						
						//! If true the user modification to the features are denied
						//bool lockedOnUserModification;

						//! Command specific delay beetween two sequence of the commadn step (acquire->correlation) in microseocnds
						uint64_t featureSchedulerStepsDelay;
						
						//! Checker time
						/*! 
							Command specific delay between runninc command state and
							Submission rule for install new command
						 */
						uint32_t featureSubmissionRetryDelay;
						
						//! Command Timeout
						/*!
						 determinate the commad timeout in microseconds, after which it will be faulty (0 - mean no timeout)
						 */
						uint64_t featureCommandTimeout;
					} Features;
                }

            }
        }
    }
}

#endif
