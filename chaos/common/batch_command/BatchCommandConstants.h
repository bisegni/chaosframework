/*
 *	BatchCommandConstants.h
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

#ifndef CHAOSFramework_BatchCommandConstants_h
#define CHAOSFramework_BatchCommandConstants_h
namespace chaos {
    namespace common {
        namespace batch_command {
            
            /** @defgroup BatchCommandSubmissionKey chaos slow control submission key
             @{
             */
            //! Name space for grupping option used for regulate the submission of the slow control
            namespace BatchCommandSubmissionKey {
                //! @The key represent the postfix to the CU instance to identify his slow command executor {not used for now}.
                static const char * const   COMMAND_EXECUTOR_POSTFIX_DOMAIN		= "_scexec";
                //! @The key represent the execution channel where submit the command, if not set, the first channel is used (1), the value is an uint32 [1-n]
                static const char * const   COMMAND_EXECUTION_CHANNEL           = "cmd_exec_channel";
                //! @The key represent the option to the command submission, the value is an uint32
                static const char * const   SUBMISSION_RULE_UI32				= "cmd_submssn_rule";
                //! @The key represent the option to the command submission priority, the value is an uint32
                static const char * const   SUBMISSION_PRIORITY_UI32            = "cmd_submssn_priority";
                //! @The key represent intervall between step of the scheduler expressed in microseconds, the value is an uint32
                static const char * const   SCHEDULER_STEP_TIME_INTERVALL_UI64  = "cmd_sched_step_time_int";
                //! @Set the command specific delay between submission retry common::submissionRetryDelay, the value is an uint32
                static const char * const   SUBMISSION_RETRY_DELAY_UI32         = "cmd_submssn_ret_dly";
            }
            /** @} */ // end of BatchCommandSubmissionKey
            
            /** @defgroup BatchCommandsKey chaos slow control default command keys
             @{
             */
            //! Name space for grupping the key nedded for the defaults slow control commands
            namespace BatchCommandsKey {
                static const char * const   DEFAULT_WAIT_COMMAND_ALIAS          = "sc_wait";
                static const char * const   DEFAULT_WAIT_COMMAND_DELAY_KEY      = "delay";
            }
            /** @} */ // end of BatchCommandsKey
            
            /** @defgroup API_Slow_Control Slow Control API
             @{
             */
            //! Name space for the key nedded for call rpc action exposed by slow control executor layer
            namespace BatchCommandExecutorRpcActionKey {
                
                //! @The key represent the rpc alias for BatchCommandExecutor::getCommandSandboxStatistics method
                static const char * const   RPC_GET_COMMAND_STATE								= "getCmdState";
                //! @The key represent the field of the input parameter tha identify the unique command id, the value is an uint64_t
                static const char * const   RPC_GET_COMMAND_STATE_CMD_ID_UI64					= "command_id";
                //! @The key represent the field of the return pack of the command RPC_GET_COMMAND_STATE for retrive the last event occurend on command, value is an uint32_t
                static const char * const   RPC_GET_COMMAND_STATE_LAST_EVENT_UI32				= "gcs_last_event";
                //! @The key represent the field of the return pack of the command RPC_GET_COMMAND_STATE for retrive the error on command, value is an int32_t
                static const char * const   RPC_GET_COMMAND_STATE_ERROR_CODE_UI32				= "gcs_error_code";
                //! @The key represent the field of the return pack of the command RPC_GET_COMMAND_STATE for retrive the error description on command, value is an string
                static const char * const   RPC_GET_COMMAND_STATE_ERROR_DESC_STR				= "gcs_error_desc";
                //! @The key represent the field of the return pack of the command RPC_GET_COMMAND_STATE for retrive the last event occurend on command, value is an string
                static const char * const   RPC_GET_COMMAND_STATE_ERROR_DOMAIN_STR				= "gcs_error_domain";
                //! @The key represent the rpc alias for BatchCommandExecutor::setCommandFeatures method
                static const char * const   RPC_SET_COMMAND_FEATURES							= "setCommandFeatures";
                //! @The key represent the the scheduler wait time for next step, value is and uint32_t
                static const char * const   RPC_SET_COMMAND_FEATURES_SCHEDULER_STEP_WAITH_UI64	= "scf_sched_waith_time";
                //! @The key represent the lock option for the features, value is and boolean
                static const char * const   RPC_SET_COMMAND_FEATURES_LOCK_BOOL					= "scf_lock";
                
                //! @The key represent the rpc alias for BatchCommandExecutor::killCurrentCommand method
                static const char * const   RPC_KILL_CURRENT_COMMAND = "killCurrentCommand";
                
                //! @The key represent the rpc alias for BatchCommandExecutor::flushCommandStates method
                static const char * const   RPC_FLUSH_COMMAND_HISTORY = "flushCmdStateHst";
            }
            /** @} */ // end of API_Slow_Control Slow Control API
            
            /** @defgroup KEY_bacth_command_description Batch Command Description Key
             @{
             */
            //! Name space for the key usign for batch command description
            namespace BatchCommandAndParameterDescriptionkey {
                //! @The key represent the alias of the batch command
                static const char * const BC_UNIQUE_ID              = "bc_unique_id";
                
                //! @The key represent the alias of the batch command
                static const char * const BC_ALIAS                  = "bc_alias";
                
                //! @The key represent the description of the command
                static const char * const BC_DESCRIPTION            = "bc_description";
                
                //! @The key represent the list of all parameters
                static const char * const BC_PARAMETERS             = "bc_parameters";
                
                //! @The key represent the paramter name
                static const char * const BC_PARAMETER_NAME         = "bc_parameter_name";
                
                //! @The key represent the paremter description
                static const char * const BC_PARAMETER_DESCRIPTION  = "bc_parameter_description";
                
                //! @The key represent the paremter type
                static const char * const BC_PARAMETER_TYPE         = "bc_parameter_type";
                
                //! @The key an int32 that contais the below describted flags
                static const char * const BC_PARAMETER_FLAG         = "bc_parameter_flag";
                
                static int BC_PARAMETER_FLAG_MANDATORY = 1;
                static int BC_PARAMETER_FLAG_OPTIONAL = 0;

            }
            /** @} */ // end of BatchCommandDescriptionKey Slow Control API
        }
    }
}
#endif
