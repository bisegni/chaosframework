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
#ifndef ChaosFramework_ConstrolSystemConstants_h
#define ChaosFramework_ConstrolSystemConstants_h

#include <chaos/common/batch_command/BatchCommandConstants.h>
#include <chaos/common/external_unit/external_unit_constants.h>

#include <string>
#include <algorithm>

namespace chaos {
    
    /** @defgroup CommonParamOption Common Layer Configuration Option
     *  This is the collection of the parameter for common layer to customize the initial setup
     *  @{
     */
    //! groups the default !CHAOS option keys used in command line or config file
    namespace InitOption{
        //! for print the help
        static const char * const	OPT_HELP                            = "help";
        //! Show the version 
        static const char * const	OPT_VERSION                         = "version";
        //! config file parameter
        static const char * const   OPT_CONF_FILE						= "conf-file";
        //! Specify when the log must be forwarded on console
        static const char * const   OPT_LOG_ON_CONSOLE                  = "log-on-console";
        //! Specify when the log must be forwarded on file
        static const char * const   OPT_LOG_ON_FILE                     = "log-on-file";
        //! Specify when the file path of the log
        static const char * const   OPT_LOG_FILE                        = "log-file";
        //! Specify when the log need to be forwarded on mds
        static const char * const   OPT_LOG_ON_MDS                      = "log-on-mds";
        //! enable logging on syslog
        static const char * const   OPT_LOG_ON_SYSLOG                   = "log-on-syslog";
        //! enable logging on syslog
        static const char * const   OPT_LOG_SYSLOG_SERVER               = "log-syslog-server";
        //! specify the port of syslogerver
        static const char * const   OPT_LOG_SYSLOG_SERVER_PORT          = "log-syslog-server-port";
        //! Specify the level of the log going
        static const char * const   OPT_LOG_LEVEL                       = "log-level";
        //! Specify the log max size
        static const char * const   OPT_LOG_MAX_SIZE_MB                 = "log-max-size";
        //! Specify the implementation to use for rp messaging
        static const char * const   OPT_RPC_IMPL_KV_PARAM               = "rpc-kv-param";
        //! Specify the implementation to use for rp messaging
        static const char * const   OPT_RPC_IMPLEMENTATION              = "rpc-server-impl";
        //! Specify the implementation to use for rp messaging
        static const char * const   OPT_RPC_SYNC_ENABLE                 = "rpc-synchronous-enable";
        //! Specify the network port where rpc system will publish al the service
        static const char * const   OPT_RPC_SERVER_PORT                 = "rpc-server-port";
        //! Specify the number of the thread that the rpc ssytem must use to process the request
        static const char * const   OPT_RPC_SERVER_THREAD_NUMBER        = "rpc-server-thread-number";
        //! Specify the number of the thread that EVERY queue in EVERY action domain has for rpc action consume
        static const char * const   OPT_RPC_DOMAIN_QUEUE_THREAD         = "rpc-domain-queue-thread-number";
        //! specify the schduler type for the execution of the rcp action(0-default,1-shared)
        static const char * const   OPT_RPC_DOMAIN_SCHEDULER_TYPE       = "rpc-domain-scheduler-type";
        //! Specify the implementation to use for the direct io subsystem
        static const char * const   OPT_DIRECT_IO_IMPLEMENTATION		= "direct-io-impl";
        //! Specify the network port where the direct io subsystem publish i's priority channel
        static const char * const   OPT_DIRECT_IO_PRIORITY_SERVER_PORT  = "direct-io-priority-server-port";
        //! Specify the network port where the direct io subsystem publish i's priority channel
        static const char * const   OPT_DIRECT_IO_SERVICE_SERVER_PORT   = "direct-io-service-server-port";
        //! Specify the number of the thread that the direct io subsystem must use to process the request
        static const char * const   OPT_DIRECT_IO_SERVER_THREAD_NUMBER  = "direct-io-server-thread-number";
        //! Specify the key/values (string:string) map for direct io server implementation parameters
        static const char * const   OPT_DIRECT_IO_SERVER_IMPL_KV_PARAM  = "direct-io-server-kv-param";
        //! Specify the key/values (string:string) map for direct io client implementation parameters
        static const char * const   OPT_DIRECT_IO_CLIENT_IMPL_KV_PARAM  = "direct-io-client-kv-param";
        //!disable the event system
        static const char * const   OPT_EVENT_DISABLE                   = "event-disable";
        //! Specify the metadata address for the metadataserver
        static const char * const   OPT_METADATASERVER_ADDRESS          = "metadata-server";
        //! Enable autoconfiguration for mds endpoint
        static const char * const   OPT_METADATASERVER_AUTO_CONF        = "metadata-server-auto-conf";
        //! Specify the ip where publish the framework
        static const char * const   OPT_PUBLISHING_IP                   = "publishing-ip";
        //! use the interface name to determinate the ip where publish itself
        static const char * const   OPT_PUBLISHING_INTERFACE            = "publishing-interface";
        //! Specify the ip where publish the framework
        static const char * const   OPT_DATA_IO_IMPL					= "data-io-impl";
        //! Enable the calibration of the local timestsamp with a remote NTP server
        static const char * const   OPT_TIME_CALIBRATION                = "time-calibration";
        //! Specify the bound of offset after wich the calibration work
        static const char * const   OPT_TIME_CALIBRATION_OFFSET_BOUND   = "time-calibration-oofset-bound";
        //! Enable the calibration of the local timestsamp with a remote NTP server
        static const char * const   OPT_TIME_CALIBRATION_NTP_SERVER     = "time-calibration-ntp-server";
        //! Enable the usage of the plugin
        static const char * const   OPT_PLUGIN_ENABLE                   = "plugin-enable";
        //! Specify the directory were can be found the plugin
        static const char * const   OPT_PLUGIN_DIRECTORY_PATH           = "plugin-directory-path";
        //!multiple key value parameter that are passed to script virtual machine
        static const char * const   OPT_SCRIPT_VM_KV_PARAM              = "script-vm-kvp";
        //!rest poll time us
        static const char * const   OPT_REST_POLL_TIME_US               = "rest-poll-us";
#if CHAOS_PROMETHEUS
        //! config file parameter
        static const char * const   OPT_METRIC_ENABLE                   = "enable_metric";
        //! config file parameter
        static const char * const   OPT_METRIC_WEB_SERVER_PORT           = "metrics-http-port";
#endif
        
    }
    /** @} */ // end of ParamOption
    
    /** @defgroup RpcConfigurationKey Rpc System Configuration
     *  This is the collection of the key for the parameter used to configura rpc system
     *  @{
     */
    //! Name space for grupping the options used for the rpc system configuration
    namespace RpcConfigurationKey {
        //! the regular expression for check the wel format key/valuparameter list for CS_CMDM_OPT_RPC_IMPL_KV_PARAM
        static const char * const OPT_RPC_IMPL_KV_PARAM_STRING_REGEX    = "([a-zA-Z0-9/_,.]+)=([a-zA-Z0-9/_,.]+)(\\|([a-zA-Z0-9/_,.]+):([a-zA-Z0-9/_,.]+))*";
        //! define the default time for rpc operation
        static const unsigned int GlobalRPCTimeoutinMSec                = 5000;
    }
    /** @} */ // end of RpcConfigurationKey
    
    namespace common {
        namespace direct_io {
            /** @defgroup DirectIOConfigurationKey DirectIO System Configuration
             *  This is the collection of the key for the parameters used to configure the DirectIO system
             *  @{
             */
            //! Name space for grupping option used for rpc system configuration
            namespace DirectIOConfigurationKey {
                //!  specify the type of the adatpre to be used
                static const char * const DIRECT_IO_IMPL_TYPE						= "direct_io_impl_type";
                //!  the number of the thread to user
                static const char * const DIRECT_IO_SERVER_THREAD_NUMBER			= "direct_io_server_tn";
                //!  specify the port where the rpc must publish the his socket
                static const char * const DIRECT_IO_PRIORITY_PORT					= "direct_io_priority_port";
                //!  specify the port where the rpc must publish the his socket
                static const char * const DIRECT_IO_SERVICE_PORT					= "direct_io_service_port";
                //!  define the default time for directio operation
                static const unsigned int GlobalDirectIOTimeoutinMSec               = 5000;
            }
            /** @} */ // end of DirectIOConfigurationKey
        }
    }
    namespace common {
        namespace constants {
            // hearth beat timers
            static const unsigned int HBTimersTimeoutinMSec                     = 5000;
            static const unsigned int AgentTimersTimeoutinMSec                  = 5000;
            static const unsigned int CUTimersTimeoutinMSec                     = 5000;
            static const unsigned int PerformanceManagerTimersTimeoutinMSec     = 5000;
            
            static const unsigned int ObjectStorageTimeoutinMSec                = 50000;
            static const unsigned int ChacheTimeoutinMSec                       = 5000;
            static const unsigned int MetricCollectorTimeoutinMSec              = 1000;
            
            //!time to wait for queue can accept new data to push in object storage
            /*!
             Mds when receive a new dataset to store on history, it is push on hst sublayer
             queue, that has a max number of element. This timeout is the max time waith by
             mds to permit queue to can accept this new dataset. after that the new dataset
             is nto accepted by sublayer.
             */
            static const unsigned int MDSHistoryQueuePushTimeoutinMSec          = 60000;
        }
        
    };
    /** @defgroup NodeDefinitionKey !CHAOS node key description
     *  This is the collection of the key for the general node information
     *  @{
     */
    //! Name space for grupping key for the node infromation
    namespace NodeDefinitionKey {
        //! define the node unique identification[string]
        /*!
         Identify in a unique way the node within a domain. A node can be
         every node except the metadata server.
         */
        static const char * const NODE_UNIQUE_ID        = "ndk_uid";
        
        //! defines the unique id that is parent of target node [string]
        /*!
         Identify an unique id that is the parent of the node
         */
        static const char * const NODE_PARENT           = "ndk_parent";
        
        //! defines at which group the node belong [array of unique id]
        /*!
         Every entry of the array is a unique id of a group
         */
        static const char * const NODE_GROUP_SET       = "ndk_group_set";
        
        //! define the node type[string]
        /*!
         The type permit to identify kind of node is. For example
         contorl unit(slow or realtime) ui, eu, etc...
         */
        static const char * const NODE_TYPE             = "ndk_type";
        
        //! define the node type[string]
        /*!
         The subtype related to the type
         */
        static const char * const NODE_SUB_TYPE         = "ndk_sub_type";
        
        //! identify the node rpc address[string:string]
        /*!
         Permit to assciate to the node the address of the rpc interface
         that is given by the network broker where the node si attacched.
         */
        static const char * const NODE_RPC_ADDR         = "ndk_rpc_addr";
        
        //! identify the node rpc address[string:string]
        /*!
         si the address direct io base address(whitout endpoint) published by the node
         */
        static const char * const NODE_DIRECT_IO_ADDR   = "ndk_d_io_addr";
        
        //! identify the domain within the rpc infrastructure [string]
        /*!
         Identify the domain where the node has registered his api.
         */
        static const char * const NODE_RPC_DOMAIN       = "ndk_rpc_dom";
        
        //! identify the node security key[string]
        /*!
         the security key is a public and private key standard
         thank permit to a node to be securily identifyed.
         */
        static const char * const NODE_SECURITY_KEY     = "ndk_sec_key";
        
        
        //! is the hartbeat of the node for the current request[uint64]
        static const char * const NODE_TIMESTAMP        = "ndk_heartbeat";
        
        //! brief node description  [string]
        static const char * const NODE_DESC             = "ndk_desc";
        
        //! brief node custom configuration parameters [CDataWrapper]
        static const char * const NODE_CUSTOM_PARAM     = "ndk_custom_param";
        
        //! brief node host name [string]
        static const char * const NODE_HOST_NAME        = "ndk_host_name";
    }
    /** @} */ // end of NodeDefinitionKey
    
    /** @defgroup NodeType !CHAOS node type value description
     *  this is the list of the all default node type recognized automatically by !CHAOS
     * infrastructure
     *  @{
     */
    //! Name space for grupping key for the node type
    namespace NodeType {
        //! identify a single process healt service
        /*!
         Every common toolkit singleton(every process) has an internal healt system
         taht register in an syncrhonous way his presence.
         */
        static const char * const NODE_TYPE_HEALT_PROCESS     = "nt_healt_process";
        
        //! identify an unit server node
        /*!
         A unit server node is a specialized node that act as server for many configurable
         constrol unit.
         */
        static const char * const NODE_TYPE_UNIT_SERVER     = "nt_unit_server";
        //! identify a control unit node
        /*!
         A CU node is a tipical !CHAOS node that act as controller of hardware of
         other chaos node.
         */
        static const char * const NODE_TYPE_CONTROL_UNIT    = "nt_control_unit";
        
        
        //! identify an user itnerface node
        /*!
         An user interface is a node that can control other nodes and show
         data from it, tipically a program achieve to monitor the system and
         send command to change the his state.
         */
        static const char * const NODE_TYPE_USER_INTERFACE  = "nt_user_interface";
        //! identify a data service node
        /*!
         A data service node is a !CHAOS service that manage the query on producer data
         */
        static const char * const NODE_TYPE_DATA_SERVICE    = "nt_data_service";
        
        //! identify a wan proxy node
        /*!
         A wan proxy node is a node that permit to adapt the wan syncrhonous worls to
         !CHAOS async one.
         */
        static const char * const NODE_TYPE_WAN_PROXY       = "nt_wan_proxy";
        
        //! identify a process agent
        /*!
         A process agent is a daemo that run on an host that permit to
         manage chaos process whitin that host (start stop uni t server and perform deploy)
         */
        static const char * const NODE_TYPE_AGENT       = "nt_agent";
        
        //! identify an execution unit of type script
        /*!
         A scriptable execution unit consinst of a framework implementaion of the an
         execution unit that permit o load script for different language.
         */
        static const char * const NODE_SUBTYPE_SCRIPTABLE_EXECUTION_UNIT    = "nt_script_eu";
        static const char * const NODE_SUBTYPE_REALTIME_CONTROL_UNIT        = "nt_rt_cu";
        static const char * const NODE_SUBTYPE_BATCH_CONTROL_UNIT           = "nt_sc_cu";
        static const char * const NODE_SUBTYPE_PROXY_CONTROL_UNIT           = "nt_proxy_cu";
        //! identify a DAQ unit node
        /*!
         A DAQ unit node is a tipical !CHAOS node that acquires data without any command
         */
        static const char * const NODE_SUBTYPE_DAQ_CONTROL_UNIT    = "nt_daq_unit";
        typedef enum  {
            //!Search us
            node_type_us = 1,
            //!Search cu
            node_type_cu = 2,
            //!search agent
            node_type_agent = 3,
            //!search cds
            node_type_cds = 4,
            //!search wan
            node_type_wan = 5
            
        } NodeSearchType;
    }
    /** @} */ // end of NodeType
    
    
    
    /** @defgroup NodeStateFlagDefinitionKey constant for serialization of the status flag
     *  @{
     */
    //! This namespace enclose all the constant for serialization of the status flag
    namespace NodeStateFlagDefinitionKey {
        //! flag name[string]
        static const char * const NODE_SF_NAME                  = "nsfdk_name";
        //! flag catalog+name[string]
        static const char * const NODE_SF_COMPOSED_NAME         = "nsfdk_composed_name";
        //! flag description[string]
        static const char * const NODE_SF_DESCRIPTION           = "nsfdk_description";
        //! flag level name[int32(char)]
        static const char * const NODE_SF_LEVEL_VALUE           = "nsfdk_level_value";
        //!contains the lis tof the level that belong to the flag[bson vector of object]
        static const char * const NODE_SF_LEVEL_SET             = "nsfdk_level_set";
        //! flag level tag[string]
        /*!
         tag is some king of minimal string code tath permit to recognize tha level
         */
        static const char * const NODE_SF_LEVEL_TAG             = "nsfdk_level_tag";
        //! flag level description [string]
        /*!
         thi spermit to fully exmplain what the level indicate
         */
        static const char * const NODE_SF_LEVEL_DESCRIPTION     = "nsfdk_level_description";
        //! flag level name [enum]
        static const char * const NODE_SF_LEVEL_SEVERITY        = "nsfdk_level_severity";
        
        //! the name of the catalog of a status flag set [string]
        static const char * const NODE_SF_CATALOG_NAME          = "nsfdk_catalog_name";
        //!define a vector of flag set within the catalog
        static const char * const NODE_SF_CATALOG_FLAG_SET      = "nsfdk_catalog_flag_set";
        //!define a serialization of status flag within the searialization set
        static const char * const NODE_SF_CATALOG_FLAG          = "nsfdk_catalog_flag";
    }
    /** @} */ // end of NodeStateFlagDefinitionKey
    
    /** @defgroup ControlUnitHealtDefinitionValue !CHAOS control unit specific key
     *  @{
     */
    //! This namespace enclose all the control unit specific healt key
    namespace ControlUnitHealtDefinitionValue {
        //!define the key that contains the rate of the output dataset pushes per second[double]
        static const char * const CU_HEALT_OUTPUT_DATASET_PUSH_RATE  = "cuh_dso_prate";
        static const char * const CU_HEALT_OUTPUT_DATASET_PUSH_SIZE  = "cuh_dso_size";
        
        
    }
    /** @} */ // end of NodeHealtDefinitionValue
    
    
    /** @defgroup NodeDomainAndActionRPC !CHAOS node rpc key description
     *  @{
     */
    //! Name space for grupping the key for action published by the node. Most of the funtion are node related so belong
    //! to a different rpc domains, but the default domain(used for action of general usage is identified by RPC_DOMAIN key)
    namespace NodeDomainAndActionRPC {
        //! The domain for the rpc action for every nodes (and sublcass used for general use)
        static const char * const RPC_DOMAIN                                        = "system";
        //! Action that needs to answer with the status of the node(specialized for  every node)
        static const char * const ACTION_NODE_STATUS                                = "ndk_rpc_request_status";
        //! Action that is called to inform the node of the registration status
        static const char * const ACTION_REGISTRATION_ACK                           = "ndk_rpc_reg_ack";
        
        //! Start the control unit intialization, the action need the default value
        //! of the input attribute for a determinate device
        static const char * const ACTION_NODE_INIT                                  = "initNode";
        
        //! Deinitialization of a control unit, if it is in run, the stop phase
        //! is started befor deinitialization one
        static const char * const ACTION_NODE_DEINIT                                = "deinitNodeUnit";
        
        //! start the run method schedule for a determinated device
        static const char * const ACTION_NODE_START                                 = "startNodeUnit";
        
        //! pause the run method for a determinated device
        static const char * const ACTION_NODE_STOP                                  = "stopNodeUnit";
        
        //! recovery a recoverable state of the node
        static const char * const ACTION_NODE_RECOVER                               = "recoverNodeUnit";
        
        //! pause the run method for a determinated device
        static const char * const ACTION_NODE_RESTORE                               = "restoreNodeUnit";
        
        //! restore the control unit to a determinate temporal tag
        static const char * const ACTION_NODE_RESTORE_PARAM_TAG                     = "restoreNodeTag";
        
        //! is the name of the temporal tag to use as restore point
        static const char * const ACTION_NODE_GET_STATE                             = "getNodeState";
        
        //! return the control unit information
        static const char * const ACTION_CU_GET_INFO                                = "getNodeInfo";
        
        //! update the node property
        static const char * const ACTION_UPDATE_PROPERTY                            = "updateProperty";
        
        //! perform an echo test, return as is the input datapach as output
        static const char * const ACTION_ECHO_TEST                                  = "echoTest";
        
        //! perform an echo test. Return as is the input datapach as output
        static const char * const ACTION_CHECK_DOMAIN                               = "checkDomain";
        
        //! get the build information form node instance
        static const char * const ACTION_GET_BUILD_INFO                             = "getBuildInfo";
        //! get the process information form node instance
        static const char * const ACTION_GET_PROCESS_INFO                             = "getProcessInfo";
        //! shutdown chaos process
        static const char * const ACTION_NODE_SHUTDOWN                           = "nodeShutDown";
    }
    /** @} */ // end of NodeDomainAndActionRPC
    
    /** @defgroup HealtProcessDefinitionKey !CHAOS healt process key description
     *  @{
     */
    namespace HealtProcessDefinitionKey {
    }
    /** @} */ // end of HealtProcessDefinitionKey
    
    /** @defgroup HealtProcessDomainAndActionRPC !CHAOS healt process key for rpc communication
     *  @{
     */
    namespace HealtProcessDomainAndActionRPC {
        //! The domain for unit server rpc action
        static const char * const RPC_DOMAIN                                        = "healt";
        //! This action is exposed by MDS and nned to be called to the remote node
        //! for publish itself
        static const char * const ACTION_PROCESS_WELCOME                            = "processHello";
        //! This action is exposed by MDS and nned to be called to the remote node
        //! for unpublish itself
        static const char * const ACTION_PROCESS_BYE                                = "processBye";
        
        
    }
    /** @} */ // end of HealtProcessDomainAndActionRPC
    
    /** @defgroup UnitServerNodeDefinitionKey !CHAOS unit server node key description
     *  @{
     */
    //! Name space for grupping key for the node type
    namespace UnitServerNodeDefinitionKey {
        
        //! key for the control unit aliases published by the unit server [array fo string]
        static const char * const UNIT_SERVER_HOSTED_CONTROL_UNIT_CLASS       = "usndk_hosted_cu_class";
        
        //! key the addresses the list of the states of the CU for a given Unit Serve   string]
        static const char * const UNIT_SERVER_HOSTED_CU_STATES                = "usndk_hosted_cu_states";
        
    }
    /** @} */ // end of UnitServerNodeDefinitionKey
    
    /** @defgroup UnitServerNodeDomainAndActionRPC !CHAOS unit server rpc key description
     *  This is the collection of all key used only by unit server
     *  @{
     */
    namespace UnitServerNodeDomainAndActionRPC {
        //! The domain for unit server rpc action
        static const char * const RPC_DOMAIN                                        = "unit_server";
        
        //! action called for load operation of the hosted control unit type
        static const char * const ACTION_UNIT_SERVER_LOAD_CONTROL_UNIT              = "unitServerLoadControlUnit";
        
        //! action called for load operation of the hosted control unit type
        static const char * const ACTION_UNIT_SERVER_UNLOAD_CONTROL_UNIT            = "unitServerUnloadControlUnit";
        
        //! Action called by mds for ack message in the unit server registration process
        static const char * const ACTION_UNIT_SERVER_REG_ACK                        = "unitServerRegistrationACK";
        
        //! Action that is called to inform the node of the registration status of the hosted control unit
        static const char * const ACTION_REGISTRATION_CONTROL_UNIT_ACK              = "ndk_rpc_reg_cu_ack";
        
        //! driver params passed during load operation for a specified control unit
        static const char * const PARAM_CU_LOAD_DRIVER_PARAMS                       = "controlUnitDriverParams";
        
        //! Alias to the intancer of the control unit to allocate [string]
        /*!
         Represent the control unit type to be load or unload
         */
        static const char * const PARAM_CONTROL_UNIT_TYPE                           = "usn_rpc_par_control_unit_type";
        
        //! List of command to execute at the startup of the control unit [vector of CDataWrapper]
        /*!
         Represent the control unit type to be load or unload
         */
        static const char * const PARAM_CONTROL_UNIT_STARTUP_COMMAND                = "usn_rpc_par_control_unit_sc";
    }
    /** @} */ // end of UnitServerNodeDomainAndActionRPC
    
    
    /** @defgroup AgentNodeDefinitionKey !CHAOS agent node key description
     *  @{
     */
    //! Name space for grupping key for the agent node type
    namespace AgentNodeDefinitionKey {
        
        //! key for the worker contained within the agent
        /*!
         very worke can process different work witni the agent
         */
        static const char * const HOSTED_WORKER       = "andk_hosted_worker";
        
        //!the name that identify the worker
        static const char * const WORKER_NAME         = "andk_worker_name";
        
        //!the description of the worker
        static const char * const WORKER_DESCRIPTION    = "andk_worker_description";
        
        //!the list of node associated with the agent
        static const char * const NODE_ASSOCIATED       = "andk_node_associated";
        
        //!the working directory of the agent instance
        static const char * const WORKING_DIRECTORY     = "andk_working_directory";
    }
    /** @} */ // end of AgentNodeDefinitionKey
    
    /** @defgroup AgentNodeDomainAndActionRPC !CHAOS agent rpc key description
     *  This is the collection of all key used only by agent
     *  @{
     */
    namespace AgentNodeDomainAndActionRPC {
        //! The domain for agent rpc action
        static const char * const RPC_DOMAIN_                                        = "agent";
        //! action called for the ack of the agent from mds
        static const char * const ACTION_AGENT_REGISTRATION_ACK                     = "agentRegistrationAck";
        
        //!identify the error code for the registration
        static const char * const REGISTRATION_RESULT                               = "andk_rpc_registration_result";
        
        namespace ProcessWorker {
            //! The domain for agent rpc action
            static const char * const RPC_DOMAIN                            = "ProcessWorker";
            static const char * const ACTION_LAUNCH_NODE                    = "startNode";
            static const char * const ACTION_LAUNCH_NODE_PAR_NAME           = NodeDefinitionKey::NODE_UNIQUE_ID;
            static const char * const ACTION_LAUNCH_NODE_CMD_LINE           = "node_launch_cmd_line";
            static const char * const ACTION_LAUNCH_NODE_PAR_CFG            = "node_init_cfg";
            static const char * const ACTION_LAUNCH_NODE_PAR_AUTO_START     = "node_auto_start";
            static const char * const ACTION_LAUNCH_NODE_PAR_KEEP_ALIVE     = "node_keep_alive";
            static const char * const ACTION_LAUNCH_NODE_PAR_LOG_ON_MDS     = "node_log_on_mds";
            
            static const char * const ACTION_STOP_NODE                      = "stopNode";
            static const char * const ACTION_STOP_NODE_PAR_NAME             = "node_name";
            
            static const char * const ACTION_RESTART_NODE                   = "restartNode";
            static const char * const ACTION_RESTART_NODE_PAR_NAME          = NodeDefinitionKey::NODE_UNIQUE_ID;
            static const char * const ACTION_RESTART_NODE_PAR_KILL          = "kill";
            
            static const char * const ACTION_LIST_NODE                      = "listNode";
            static const char * const ACTION_LIST_NODE_PARM_NAME            = NodeDefinitionKey::NODE_UNIQUE_ID;
            
            static const char * const ACTION_CHECK_NODE                     = "checkNode";
            static const char * const ACTION_CHECK_NODE_ASSOCIATED_NODES    = AgentNodeDefinitionKey::NODE_ASSOCIATED;
            static const char * const ACTION_CHECK_NODE_RESULT_NODE_UID     = NodeDefinitionKey::NODE_UNIQUE_ID;
            static const char * const ACTION_CHECK_NODE_RESULT_NODE_ALIVE   = "alive";
        }
        
        namespace DeployWorker {
            static const char * const RPC_DOMAIN                    = "DeployWorker";
            static const char * const ACTION_INIT_DEPLOY_SESSION    = "initDeploySession";
            static const char * const ACTION_UPLOAD_DEPLOY_CHUNK    = "uploadDeployChunk";
            static const char * const ACTION_END_DEPLOY_SESSION     = "endDeploySession";
            
            static const char * const ACTION_PARAM_SESSION_ID       = "session_uid";
            static const char * const ACTION_PARAM_CHUNK_NUMBER     = "chunk_number";
            static const char * const ACTION_PARAM_CHUNK_OFFSET     = "chunk_offset";
            static const char * const ACTION_PARAM_CHUNK_DATA       = "chunk_data";
            static const char * const ACTION_PARAM_SESSION_HASH     = "session_hash";
        }
        
        namespace LogWorker {
            static const char * const RPC_DOMAIN                            = "LogWorker";
            static const char * const ACTION_START_LOGGING_ASSOCIATION  = "startLoggingAssociation";
            static const char * const ACTION_STOP_LOGGING_ASSOCIATION   = "stopLoggingAssociation";
        }
    }
    /** @} */ // end of AgentNodeDomainAndActionRPC
    
    /** @defgroup DataServiceNodeDefinitionKey !CHAOS data service node key description
     *  This is the collection of the key used to configure the DataProxy server
     *  address and port and how the client's(cu and ui) need to access to it (round robin or fail over)
     *  @{
     */
    //! This is the collection of the key to configura history and live channel
    namespace DataServiceNodeDefinitionKey {
        static const char * const DS_HA_ZONE                                        = "dsndk_ha_zone";
        static const char * const DS_DIRECT_IO_FULL_ADDRESS_LIST                    = "dsndk_direct_io_full";
        //!lis the endpoitwhere is published the direct io[uint32_t]
        static const char * const DS_DIRECT_IO_ENDPOINT                             = "dsndk_direct_io_ep";
        //!define the type of storage(history only = 0, cache only	= 1, both	= 2) [uint32_t]
        static const char * const DS_STORAGE_TYPE                                   = "dsndk_storage_type";
        //!define the numbers of second for the ageing time, that is maximu number of second
        //!for wich the data need to remain archive within the !CHAOS storage system. A value
        //! of 0 mean infinit persistence[uint32_t]
        static const char * const DS_STORAGE_HISTORY_AGEING                         = "dsndk_storage_history_ageing";
        /*!
         storage time [in msecods uint64] the time that need to pass before
         store another datapack into the history system[uint64]
         */
        static const char * const DS_STORAGE_HISTORY_TIME                           = "dsndk_storage_history_time";
        /*!
         storage time [in msecods uint64] the time that need to pass before
         store another datapack into the live system[uint64]
         */
        static const char * const DS_STORAGE_LIVE_TIME                              = "dsndk_storage_live_time";
        
    }
    /** @} */ // end of DataServiceNodeDefinitionKey
    
    /** @defgroup DataServiceNodeDefinitionType !CHAOS data service node typ for key
     *  This is the collection of the type used as value for one or more configuration
     *  @{
     */
    namespace DataServiceNodeDefinitionType {
        //!define the type of storage within a datasource
        /*!
         The direct io can be configura to change the storage option
         so every datapack can be considere with different store behaviour
         */
        typedef enum DSStorageType {
            //!no storage behaviour defined
            DSStorageTypeUndefined = 0,
            //!the datapack is store into the storage system and not in live
            DSStorageTypeHistory = 1,
            //!the datapack is publish in live shared memory
            DSStorageTypeLive = 2,
            //!datapack is published and stored in live and history
            DSStorageTypeLiveHistory = 3
        } DSStorageType;
    }
    /** @} */ // end of DataServiceNodeDefinitionType
    
    /** @defgroup DataServiceNodeDomainAndActionRPC !CHAOS data service rpc key description
     *  This is the collection of all key used only by unit server
     *  @{
     */
    namespace DataServiceNodeDomainAndActionRPC {
        //! The domain for unit server rpc action
        static const char * const RPC_DOMAIN                                    = "data_service";
    }
    /** @} */ // end of DataServiceNodeDomainAndActionRPC
    
    /** @defgroup ControlUnitNodeDefinitionKey List of control unit node type attribute key
     *  @{
     */
    //! Name space for grupping key for the control unit node type
    namespace ControlUnitNodeDefinitionKey {
        //! param to pass to the control unit during load operation[ string]
        static const char * const CONTROL_UNIT_LOAD_PARAM                           = "cudk_load_param";
        //! param to pass to the control unit during init operation[ int64]
        static const char * const CONTROL_UNIT_RUN_ID                               = "cudk_run_id";
        //! Description for the control unit dirvers [vector[string, string, string]*]
        static const char * const CONTROL_UNIT_DRIVER_DESCRIPTION                   = "cudk_driver_description";
        
        //! The name of the driver to use[strig]
        static const char * const CONTROL_UNIT_DRIVER_DESCRIPTION_NAME              = "cudk_driver_description_name";
        
        //! The version of the driver to use[strig]
        static const char * const CONTROL_UNIT_DRIVER_DESCRIPTION_VERSION           = "cudk_driver_description_version";
        
        //! The version of the driver to use[strig]
        static const char * const CONTROL_UNIT_DRIVER_DESCRIPTION_INIT_PARAMETER	= "cudk_driver_description_init_parameter";
        
        //!key for dataset description (array of per-attribute document)
        static const char * const CONTROL_UNIT_DATASET_DESCRIPTION                  = "cudk_ds_desc";
        
        //!key for dataset name
        static const char * const CONTROL_UNIT_DATASET_NAME                         = "cudk_ds_name";
        
        //!key for dataset type as @chaos::DataType::DatasetType
        static const char * const CONTROL_UNIT_DATASET_TYPE                         = "cudk_ds_type";
        
        //!key that represent the key to use to search dataset on the shared memory
        static const char * const CONTROL_UNIT_DATASET_SEARCH_KEY                   = "cudk_ds_search_key";
        
        //!key that represent the key to use to search dataset on the shared memory
        static const char * const CONTROL_UNIT_DATASET_ATTRIBUTE_LIST               = "cudk_ds_search_attribute_list";
        
        //!key for dataset timestampt validity[uint64_t]
        static const char * const CONTROL_UNIT_DATASET_TIMESTAMP                    = "cudk_ds_timestamp";
        
        //!key for the name of dataset attribute
        static const char * const CONTROL_UNIT_DATASET_ATTRIBUTE_NAME               = "cudk_ds_attr_name";
        
        //!key representing the type of parameter
        static const char * const CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE               = "cudk_ds_attr_type";
        
        //!key for the units ofr the attrbiute (ampere, volts)
        static const char * const CONTROL_UNIT_DATASET_ATTRIBUTE_UNIT               = "cudk_ds_attr_unit";
        
        //!key representing the name of the parameter
        static const char * const CONTROL_UNIT_DATASET_ATTRIBUTE_DESCRIPTION        = "cudk_ds_attr_desc";
        
        //!key representig the information for the parameter
        static const char * const CONTROL_UNIT_DATASET_ATTRIBUTE_DIRECTION          = "cudk_ds_attr_dir";
        
        //!key representing the value max size where need (type different from raw data type ex: int32)
        static const char * const CONTROL_UNIT_DATASET_VALUE_MAX_SIZE               = "cudk_ds_max_size";
        
        //!key representing the default value
        static const char * const CONTROL_UNIT_DATASET_DEFAULT_VALUE                = "cudk_default_value";
        
        //!key representing the default value
        static const char * const CONTROL_UNIT_DATASET_MAX_RANGE                    = "cudk_ds_max_range";
        
        //!key representing the default value
        static const char * const CONTROL_UNIT_DATASET_MIN_RANGE                    = "cudk_ds_min_range";
        
        //!key representing the bitmask flasgs of @DataType::DataTypeModfier that are applied on data type
        static const char * const CONTROL_UNIT_DATASET_MODIFIER                     = "cudk_ds_mod";
        
        //!key representing an array of type that describe the single or multiple subtype
        //that compese a single element whitin the binary data (int32 or array of int32)
        static const char * const CONTROL_UNIT_DATASET_BINARY_SUBTYPE               = "cudk_ds_bin_st";
        
        //!key representing how many times the binary subtype is repeated within the bunary data[int32]
        static const char * const CONTROL_UNIT_DATASET_BINARY_CARDINALITY           = "cudk_ds_bin_card";
        
        //!key representing the mime string associated to the MIME subtype
        static const char * const CONTROL_UNIT_DATASET_BINARY_MIME_DESCRIPTION      = "cudk_ds_bin_mime";
        
        //!key representing the standard mime type string, that describe the data
        //!within the binary field, associated to the MIME subtype
        static const char * const CONTROL_UNIT_DATASET_BINARY_MIME_ENCODING         = "cudk_ds_bin_encoding";
        
        //!The key represent an array with the object taht represent, each one, the command description array[object...]
        static const char * const CONTROL_UNIT_DATASET_COMMAND_DESCRIPTION         = "cudk_ds_command_description";
        /*!
         Whatever storage type is actually in use, the set of this property will send data to
         the history engine with the type and value specified by the cdata wrapper seriled in it
         */
        static const char * const CONTROL_UNIT_DATASET_HISTORY_BURST               = "dsndk_history_burst";
        //! Is the key used for specify the type f burts function
        static const char * const CONTROL_UNIT_DATASET_HISTORY_BURST_TYPE          = "dsndk_history_burst_type";
        //! is the value related to the type
        static const char * const CONTROL_UNIT_DATASET_HISTORY_BURST_VALUE         = "dsndk_history_burst_value";
        //! is the tag associated to the burst
        static const char * const CONTROL_UNIT_DATASET_HISTORY_BURST_TAG           = "dsndk_history_burst_tag";
    }
    /** @} */ // end of ControlUnitNodeDefinitionKey
    
    /** @defgroup ControlUnitNodeDefinitionKey List of control unit node type attribute key
     *  @{
     */
    //! Name space for grupping key for the control unit node type
    namespace ControlUnitNodeDefinitionType {
        //! define the tipe of hisoty burst function
        typedef enum DSStorageBurstType {
            //!no storage behaviour defined
            DSStorageBurstTypeUndefined = 0,
            DSStorageBurstTypeNPush,
            DSStorageBurstTypeMSec
        } DSStorageBurstType;
        
        typedef enum SetpointState {
            //!no storage behaviour defined
            SetpointErrorException = -100,
            SetpointErrorOnRunning = -3,
            SetpointErrorOnStart = -2,
            SetpointErrorOnInit = -1,
            SetpointUnknown = 0,
            SetpointRestoreStarted=1,
            SetpointRestoreRunning=2,
            SetpointRestoreReached=3
        } SetpointState;
    }
    /** @} */ // end of ControlUnitNodeDefinitionKey
    namespace ControlUnitNodeDomainAndActionRPC {
        //!Alias associated to thefunction that apply the value changes set to the input dataset attribute
        static const char * const CONTROL_UNIT_APPLY_INPUT_DATASET_ATTRIBUTE_CHANGE_SET  = "cunrpc_ida_cs";
        
        //! Deinitialization of a control unit, if it is in run, the stop phase
        //! is started befor deinitialization one
        static const char * const ACTION_STORAGE_BURST  = "cunrpc_start_storage_burst";
        
        static const char * const ACTION_DATASET_TAG_MANAGEMENT  = "cunrpc_dataset_tag_management";
        static const char * const ACTION_DATASET_TAG_MANAGEMENT_ADD_LIST  = "add";
        static const char * const ACTION_DATASET_TAG_MANAGEMENT_REMOVE_LIST  = "remove";
    }
    
    /** @defgroup ExecutionUnitNodeDefinitionKey List of execution unit node type attribute key
     *  @{
     */
    //! Name space for grupping key for the execution unit node type
    namespace ExecutionUnitNodeDefinitionKey {
        //! array of object that describe the input variable [string]
        static const char * const EXECUTION_UNIT_DESCRIPTION            = "eudk_description";
        //!the list of the pool managed by a unit server
        static const char * const EXECUTION_POOL_LIST                   = "eudk_pool_list";
        //!the list of the instance (fisical node that identify the xecution unit based on script)
        static const char * const EXECUTION_SCRIPT_INSTANCE_LIST        = "eudk_script_instance_list";
        //!is the language that represent the script of the execution unit
        static const char * const EXECUTION_SCRIPT_INSTANCE_LANGUAGE    = "eudk_script_language";
        //!is the content of the script to be execution
        static const char * const EXECUTION_SCRIPT_INSTANCE_CONTENT     = "eudk_script_content";
        //! the dataset attribute associated to the variable[string]
        static const char * const EXECUTION_UNIT_VAR_DATASET_ATTRIBUTE  = ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME;
        //! the dataset attribute associated to the variable[string]
        static const char * const EXECUTION_UNIT_VAR_NODE_UID           = NodeDefinitionKey::NODE_UNIQUE_ID;
    }
    /** @} */ // end of ExecutionUnitNodeDefinitionKey
    
    /** @defgroup Contorl unit property key
     *  This is the collection of the key representing the property that are exposed by control unit
     *  @{
     */
    //! Name space for grupping control unit system property
    namespace ControlUnitPropertyKey {
        static const char * const P_GROUP_NAME                            = "property_abstract_control_unit";
        //! represent the type of initialization restore
        static const char * const INIT_RESTORE_OPTION                   = "cudk_init_restore_option";
        static const unsigned int INIT_RESTORE_OPTION_TYPE_STATIC       = 0;
        static const unsigned int INIT_RESTORE_OPTION_TYPE_LAST_VALIDE  = 1;
        static const char * const INIT_RESTORE_APPLY                    = "cudk_init_restore_apply";
    }
    
    /** @defgroup Contorl unit system key
     *  This is the collection of the key representing the property that are exposed into system dataset
     *  @{
     */
    //! Name space for grupping control unit system property
    namespace ControlUnitDatapackSystemKey {
        //! represent the delay beetwen a subseguent cu start method call it is a property of a control unit
        static const char * const THREAD_SCHEDULE_DELAY                   = "cudk_thr_sch_delay";
        //!represent the bypass state of the control unit
        /*!
         in this state all driver reject the command that are sent by control unit implementation
         and no action are take on the hardware
         */
        static const char * const BYPASS_STATE                            = "cudk_bypass_state";
        //!represent the bypass state of the control unit
        /*!
         Represent in system datase the state of bursting data on a control unit. The burst
         operation of a CU determinate the force of storage data on history backend for a determinated
         amount of time or cu cicle
         */
        static const char * const BURST_STATE               = "cudk_burst_state";
        //! is the tag associated to the current burst oepration
        static const char * const BURST_TAG                 = "cudk_burst_tag";
        //! is the device alarm state (0=no alarm)
        static const char * const DEV_ALRM_LEVEL           = "cudk_dalrm_lvl";
        //! is the cu alarm level state (0= no alarm)
        static const char * const CU_ALRM_LEVEL            = "cudk_calrm_lvl";
        //! is the snapshot/setpoint restore state (0=nothing,1=started,2=running,3=end,-1 error [sticky till next operation])
        static const char * const SETPOINT_STATE               = "cudk_set_state";
        //! is the snapshot/setpoint set point last tag
        static const char * const SETPOINT_TAG                 = "cudk_set_tag";
        //! is the tag associated to the current running command alias(in the case of slow contro unit)
        static const char * const RUNNING_COMMAND_ALIAS     = "running_cmd_alias";
        
        
    }
    
    /** @defgroup Contorl unit system key
     *  This is the collection of the key representing the property that are exposed into system dataset
     *  @{
     */
    //! Name space for grupping control unit system property
    namespace ControlUnitDatapackCommonKey {
        //!specify an initialization id for the node[int64]
        static const char * const RUN_ID                          = ControlUnitNodeDefinitionKey::CONTROL_UNIT_RUN_ID;
    }
    
    /** @} */ // end of ControlUnitNodeDefinitionKey
    
    /** @defgroup CUType Control Unit Default Type
     *  This is the collection of the key for the classification of the control unit types
     *  @{
     */
    //! Name space for grupping control unit types
    namespace CUType {
        static const char * const RTCU	= NodeType::NODE_SUBTYPE_REALTIME_CONTROL_UNIT;
        static const char * const SCCU  = NodeType::NODE_SUBTYPE_BATCH_CONTROL_UNIT;
        static const char * const SEXUT = NodeType::NODE_SUBTYPE_SCRIPTABLE_EXECUTION_UNIT;
        static const char * const PROXY_CU	= NodeType::NODE_SUBTYPE_PROXY_CONTROL_UNIT;
    }
    /** @} */ // end of CUType
    
    /** @defgroup ChaosDataType Chaos Basic datatype
     *  This is the collection of the definition of the chaos basic datatype
     *  @{
     */
    //! Name space for grupping the definition of the chaos basic datatype
    namespace DataType {
        //!typede for datatype
        typedef enum DataType {
            //!bool variable length
            TYPE_BOOLEAN = 0,
            //!Integer 32 bit length
            TYPE_INT32,
            //!Integer 64 bit length
            TYPE_INT64,
            //!Double 64 bit length
            TYPE_DOUBLE,
            //!C string variable length
            TYPE_STRING,
            //!byte array variable length
            TYPE_BYTEARRAY,
            
            TYPE_CLUSTER,
            //!modifier to be ored to normal data types
            TYPE_ACCESS_ARRAY=0x100,
            TYPE_UNDEFINED
        } DataType;
        
        
        //! return the name of the type by the index
        static std::string inline typeDescriptionByCode(DataType type) {
            switch(type) {
                    //!bool variable length
                case TYPE_BOOLEAN:
                    return "Boolean";
                    //!Integer 32 bit length
                case TYPE_INT32:
                    return "Int32";
                    //!Integer 64 bit length
                case TYPE_INT64:
                    return "Int64";
                    //!Double 64 bit length
                case TYPE_DOUBLE:
                    return "Double";
                    //!C string variable length
                case TYPE_STRING:
                    return "String";
                    //!byte array variable length
                case TYPE_CLUSTER:
                    return "cluster";
                    //!byte array variable length
                    
                case TYPE_BYTEARRAY:
                    return "Binary";
                default:
                    return "Undefined";
            }
        }
        
        //! return the name of the type by the index
        static DataType inline typeCodeByDescription(const std::string& _type_description) {
            std::string type_description = _type_description;
            std::transform(type_description.begin(), type_description.end(), type_description.begin(), ::tolower);
            if(type_description.compare("boolean") == 0) {
                return TYPE_BOOLEAN;
            } else if(type_description.compare("int32") == 0) {
                return TYPE_INT32;
            } else if(type_description.compare("int64") == 0) {
                return TYPE_INT64;
            } else if(type_description.compare("double") == 0) {
                return TYPE_BOOLEAN;
            } else if(type_description.compare("string") == 0) {
                return TYPE_STRING;
            } else if(type_description.compare("cluster") == 0) {
                return TYPE_CLUSTER;
            } else if(type_description.compare("binary") == 0) {
                return TYPE_BYTEARRAY;
            } else {
                return TYPE_UNDEFINED;
            }
        }
        
        typedef enum BinarySubtype {
            //!bool variable length
            SUB_TYPE_BOOLEAN = 0,
            //!Integer char bit length
            SUB_TYPE_CHAR,
            //!Integer 8 bit length
            SUB_TYPE_INT8,
            //!Integer 16 bit length
            SUB_TYPE_INT16,
            //!Integer 32 bit length
            SUB_TYPE_INT32,
            //!Integer 64 bit length
            SUB_TYPE_INT64,
            //!Double 64 bit length
            SUB_TYPE_DOUBLE,
            //!C string variable length
            SUB_TYPE_STRING,
            //! the subtype is represented by a specific mime type tagged in specific dataset constants
            SUB_TYPE_MIME,
            //! no specific encoding
            SUB_TYPE_NONE,
            //! unsigned flag
            SUB_TYPE_UNSIGNED = 0x200,
        } BinarySubtype;
        
        //subtype helper macro
#define CHAOS_SUBTYPE_IS_UNSIGNED(s)    ((s & chaos::DataType::SUB_TYPE_UNSIGNED) == chaos::DataType::SUB_TYPE_UNSIGNED)
#define CHAOS_SUBTYPE_UNWRAP(s)         (s & ~chaos::DataType::SUB_TYPE_UNSIGNED)
#define CHAOS_SUBTYPE_SET_UNSIGNED(s)   (s |= chaos::DataType::SUB_TYPE_UNSIGNED)
#define CHAOS_SUBTYPE_SET_SIGNED(s)     (s &= ~chaos::DataType::SUB_TYPE_UNSIGNED)
        
        //! return the name of the type by the index
        static std::string inline subtypeDescriptionByIndex( unsigned int sub_type) {
            BinarySubtype _subtype = (BinarySubtype)CHAOS_SUBTYPE_UNWRAP(sub_type);
            switch(_subtype) {
                case SUB_TYPE_BOOLEAN:
                    return "Boolean";
                    //!Integer char bit length
                case SUB_TYPE_CHAR:
                    return "Char";
                    //!Integer 8 bit length
                case SUB_TYPE_INT8:
                    return "INt8";
                    //!Integer 16 bit length
                case SUB_TYPE_INT16:
                    return "Int16";
                    //!Integer 32 bit length
                case SUB_TYPE_INT32:
                    return "Int32";
                    //!Integer 64 bit length
                case SUB_TYPE_INT64:
                    return "Int64";
                    //!Double 64 bit length
                case SUB_TYPE_DOUBLE:
                    return "Double";
                    //!C string variable length
                case SUB_TYPE_STRING:
                    return "String";
                    //! the subtype is represented by a specific mime type tagged in specific dataset constants
                case SUB_TYPE_MIME:
                    return "Mime";
                    //! no specific encoding
                case SUB_TYPE_NONE:
                    return "Undefined";
                default:
                    return "Invalid";
            }
        }
        
        //! return the name of the type by the index
        static BinarySubtype inline subtypeCodeByDescription(const std::string& _subtype_description) {
            std::string subtype_description = _subtype_description;
            std::transform(subtype_description.begin(), subtype_description.end(), subtype_description.begin(), ::tolower);
            
            if(subtype_description.compare("boolean") == 0) {
                return SUB_TYPE_BOOLEAN;
            } else if(subtype_description.compare("char") == 0) {
                return SUB_TYPE_CHAR;
            } else if(subtype_description.compare("int8") == 0) {
                return SUB_TYPE_INT8;
            } else if(subtype_description.compare("int16") == 0) {
                return SUB_TYPE_INT16;
            } else if(subtype_description.compare("int32") == 0) {
                return SUB_TYPE_INT32;
            } else if(subtype_description.compare("int64") == 0) {
                return SUB_TYPE_INT64;
            } else if(subtype_description.compare("double") == 0) {
                return SUB_TYPE_DOUBLE;
            } else if(subtype_description.compare("string") == 0) {
                return SUB_TYPE_STRING;
            } else if(subtype_description.compare("mime") == 0) {
                return SUB_TYPE_MIME;
            } else if(subtype_description.compare("undefined") == 0) {
                return SUB_TYPE_NONE;
            } else {
                return SUB_TYPE_NONE;
            }
        }
        
        typedef enum DataTypeModfier {
            //!bool variable length
            MODIFIER_UNSIGNED = 0
        } DataTypeModfier;
        
        //!define the direction of dataset element
        typedef enum DataSetAttributeIOAttribute{
            //!define an attribute in input
            Input = 0,
            //!define an attribute in output
            Output=1,
            //!define an attribute with two directions
            Bidirectional=2,
            Undefined
        } DataSetAttributeIOAttribute;
        
        //!define the differt types of a dataset
        typedef enum DatasetType {
            //!define an input dataset
            DatasetTypeInput = 0,
            //!define an output dataset
            DatasetTypeOutput=1,
            //!define an anum variable dataset that define states
            DatasetTypeState=2,
            //!undefined dataset
            DatasetTypeUndefined
        } DatasetType;
    }
    
    /** @defgroup CUStateKey Control Unit State
     *  This is the collection of the key for the control unit state definition
     *  @{
     */
    //! Name space for grupping option used for define the control unit state
    namespace CUStateKey {
        //! The state of the control unit
        static const char * const CONTROL_UNIT_STATE    = "cu_state";
        
        //!define states of the control unit
        /*!
         These enumeration represents the state of the control unit, the order match permit to map these to
         those of the stat machine internally defined into teh chaos::utility::StartableService.
         */
        typedef enum {
            //! define the node in uninitialized
            DEINIT  = 0,
            //! define the node is initialized and is ready to start
            INIT    = 1,
            //! define the node is running
            START   = 2,
            //! define the node has been stopped
            STOP    = 3,
            //!define an error state of the node, in error state the node wait until someone clear the error and put it again in START/STOP/DEINIT
            RECOVERABLE_ERROR = 4,
            //!define an error state of the node, in this case the error can't be recovered so it is equivalent to a deinit state
            FATAL_ERROR = 5,
            //!define the status of the node cannot be retrieved
            UNDEFINED
        } ControlUnitState;
    }
    /** @} */ // end of CUStateKey
    
    /** @defgroup MetadataServerApiKey list of key used specificately on api
     *  @{
     */
    //! Name space for grupping the key for action published by the mds node
    namespace MetadataServerApiKey {
        //!scrip api consstants
        namespace script {
            namespace search_script {
                static const char * const  FOUND_SCRIPT_LIST = "found_script_list";
            }
        }
    }
    /** @} */ // end of MetadataServerApiKey
    
    /** @defgroup MetadataServerNodeDefinitionKeyRPC List of mds node rpc action
     *  @{
     */
    //! Name space for grupping the key for action published by the mds node
    namespace MetadataServerNodeDefinitionKeyRPC {
        
        //! the key for the node registration[specific bson pack for every kind of node]
        static const char * const ACTION_REGISTER_NODE          = "mdsndk_rpc_a_reg_node";
        
        //! key that idetify the result of the node registration[int32]
        static const char * const PARAM_REGISTER_NODE_RESULT    = "mdsndk_rpc_p_reg_result";
        
        //! the key for the control unit load completion registration
        static const char * const ACTION_NODE_LOAD_COMPLETION   = "mdsndk_rpc_a_load_completion_node";
        
        //! the key for the node health status
        static const char * const ACTION_NODE_HEALTH_STATUS     = "mdsndk_rpc_a_health_status";
    }
    /** @} */ // end of NodeDomainAndActionRPC
    
    
    /** @defgroup MetadataServerLoggingDefinitionKeyRPC List of mds node log rpc action
     *  @{
     */
    //! Name space for grupping the key used for mds logging
    namespace MetadataServerLoggingDefinitionKeyRPC {
        //! the key for the domain used by the mds for publish logging api
        static const char * const ACTION_NODE_LOGGING_RPC_DOMAIN            = "mdsndk_nl_rpc_dom";
        
        //! loggin action for the submittion of the log entry
        static const char * const ACTION_NODE_LOGGING_SUBMIT_ENTRY          = "mdsndk_nl_sub_entry";
        
        //! the key represent the log source identifier [string]
        static const char * const PARAM_NODE_LOGGING_LOG_SOURCE_IDENTIFIER  = "mdsndk_nl_sid";
        
        //! the key represent the log timestamp [uint64_t]
        static const char * const PARAM_NODE_LOGGING_LOG_TIMESTAMP          = "mdsndk_nl_lts";
        
        //! the key represent the log domain of the log entry [string]
        static const char * const PARAM_NODE_LOGGING_LOG_DOMAIN             = "mdsndk_nl_ld";
        
        //! the key represent the log subject of the entry [string]
        static const char * const PARAM_NODE_LOGGING_LOG_SUBJECT            = "mdsndk_nl_lsubj";
        
        namespace ErrorLogging {
            //! the key represent the error code [int32_t]
            static const char * const PARAM_NODE_LOGGING_LOG_ERROR_CODE     = "mdsndk_nl_e_ec";
            
            //! the key represent the error message [string]
            static const char * const PARAM_NODE_LOGGING_LOG_ERROR_MESSAGE  = "mdsndk_nl_e_em";
            
            //! the key represent the error domain [string]
            static const char * const PARAM_NODE_LOGGING_LOG_ERROR_DOMAIN   = "mdsndk_nl_e_ed";
        }
        
        namespace StandardLogging {
            //! the key represent the error code [int32_t]
            static const char * const PARAM_NODE_LOGGING_LOG_LEVEL_CODE                 = "mdsndk_nl_l_lc";
            //! the key represent the error code [string]
            static const char * const PARAM_NODE_LOGGING_LOG_LEVEL_DESCRIPTION          = "mdsndk_nl_l_ld";
            //! the key represent the error message [string]
            static const char * const PARAM_NODE_LOGGING_LOG_MESSAGE                    = "mdsndk_nl_l_m";
        }
        
        namespace AlarmLogging {
            //! the key represent the alarm level code(an alarm or warning can have more severity) [int32_t]
            static const char * const PARAM_NODE_LOGGING_ALARM_LEVEL_CODE               = "mdsndk_nl_a_lc";
            //! the key represent the alarm level description [string]
            static const char * const PARAM_NODE_LOGGING_ALARM_LEVEL_DESCRIPTION        = "mdsndk_nl_a_ld";
            //! the key represent the alarm code [int32_t]
            static const char * const PARAM_NODE_LOGGING_ALARM_CODE                     = "mdsndk_nl_a_c";
            //! the key represent the specific tagging using by sublayer [string]
            static const char * const PARAM_NODE_LOGGING_ALARM_TAG                      = "mdsndk_nl_a_t";
            //! the key represent the the name of the alarm [string]
            static const char * const PARAM_NODE_LOGGING_ALARM_NAME                     = "mdsndk_nl_a_n";
            //! the key represent the the description of the alarm [string]
            static const char * const PARAM_NODE_LOGGING_ALARM_DESCRIPTION              = "mdsndk_nl_a_d";
            //! the key represent the alarms repetition witin samples [uint32_t]
            static const char * const PARAM_NODE_LOGGING_ALARM_REPETITION               = "mdsndk_nl_a_rep";
        }
        
        namespace CommandLogging {
            //! the key represent the unique execution id of the command [uint64_t]
            static const char * const PARAM_NODE_LOGGING_LOG_COMMAND_ID     = "mdsndk_nl_c_id";
            
            //! the key represent the state of the command [int32_t]
            static const char * const PARAM_NODE_LOGGING_LOG_COMMAND_STATE = "mdsndk_nl_c_s";
            
            //! the key represent descirption of the state [string]
            static const char * const PARAM_NODE_LOGGING_LOG_COMMAND_STATE_DESCRIPTION = "mdsndk_nl_c_s_desc";
            
            //! the key represent the running property of the command [int32_t]
            static const char * const PARAM_NODE_LOGGING_LOG_COMMAND_RUN_PROPERTY = "mdsndk_nl_c_rp";
            
            //! the key represent descirption of the running property [string]
            static const char * const PARAM_NODE_LOGGING_LOG_COMMAND_RUN_PROPERTY_DESCRIPTION = "mdsndk_nl_c_rp_desc";
        }
    }
    /** @} */ // end of MetadataServerLoggingDefinitionKeyRPC
    
    /** @defgroup DataPackCommonKey Chaos Data Pack common key
     This is the collection of the common key that are contained into the
     all the dataset of a data producer
     @{
     */
    namespace DataPackCommonKey {
        //!define the device unique key, this represent the primary key of the producer[string]
        static const char * const DPCK_DEVICE_ID                       = chaos::NodeDefinitionKey::NODE_UNIQUE_ID;
        //!define the device data pack sequence id unique for device, shared among all packet sent by the node[int64]
        static const char * const DPCK_SEQ_ID                          = "dpck_seq_id";
        //!this define the acquisition timestamp of the data represented by the dataset[uint64_t]
        static const char * const DPCK_TIMESTAMP                       = "dpck_ats";//chaos::NodeDefinitionKey::NODE_TIMESTAMP;
                                                                                    //!this define a custom counter often used for the hig resolution timestamp imformation
        static const char * const DPCK_HIGH_RESOLUTION_TIMESTAMP       = "dpck_hr_ats";//chaos::NodeDefinitionKey::NODE_TIMESTAMP;
                                                                                       //!define the type of the dataset uint32_t [output(0) - input(1) - custom(2) - system(3) - ....others int32_t]
        static const char * const DPCK_DATASET_TYPE                    = "dpck_ds_type";
        //!define the list of tags associated to the datapack
        static const char * const DPCK_DATASET_TAGS                    = "dpck_ds_tag";
        //! the constant that represent the output dataset type
        static const unsigned int DPCK_DATASET_TYPE_OUTPUT             = 0;
        //! the constant that represent the input dataset type
        static const unsigned int DPCK_DATASET_TYPE_INPUT              = 1;
        //! the constant that represent the custom dataset type
        static const unsigned int DPCK_DATASET_TYPE_CUSTOM             = 2;
        //! the constant that represent the system dataset type
        static const unsigned int DPCK_DATASET_TYPE_SYSTEM             = 3;
        //! the constant that represent the health dataset type
        static const unsigned int DPCK_DATASET_TYPE_HEALTH             = 4;
        //! the constant that represent the alarm dataset type
        static const unsigned int DPCK_DATASET_TYPE_DEV_ALARM          = 5;
        //! the constant that represent the alarm dataset type
        static const unsigned int DPCK_DATASET_TYPE_CU_ALARM           = 6;
        //!define tags associated to the dataset[array of string]
        static const char * const DPCK_DATASET_TAG                    = "dpck_ds_tag";
    }
    
    
    
    /** @defgroup NodeHealtDefinitionKey !CHAOS node healt key description
     *  @{
     */
    //! This namespace enclose all the key for inspetting a node healt
    //! every different node need to expose default key and custom
    namespace NodeHealtDefinitionKey {
        static const char * const HEALT_KEY_POSTFIX                     = "_healt";
        //! define time stamp of the push (usefull for heart beating) [uint64_t]
        static const char * const NODE_HEALT_TIMESTAMP                  = DataPackCommonKey::DPCK_TIMESTAMP;
        //! define time stamp of mds at time that health datapack has been received [uint64_t]
        static const char * const NODE_HEALT_MDS_TIMESTAMP              = "dpck_mds_ats";
        //! identify the time spent as user by the process and all his thread[double]
        static const char * const NODE_HEALT_USER_TIME                  = "nh_ut";
        //! identify the time spent for system call by the process and all his thread[double]
        static const char * const NODE_HEALT_SYSTEM_TIME                = "nh_st";
        //! identify the swap memory used by process[int64_t]
        static const char * const NODE_HEALT_PROCESS_SWAP               = "nh_sw";
        //! identify the uptime of the node[uint64_t]
        static const char * const NODE_HEALT_PROCESS_UPTIME             = "nh_upt";
        //! define time stamp of the collection of the last insert metric
        static const char * const NODE_HEALT_TIMESTAMP_LAST_METRIC      = "nh_ts_lst_metric";
        //! define the status of a node (loaded, initilized, deinitialized, started, stopped, unloaded)
        static const char * const NODE_HEALT_STATUS                     = "nh_status";
        //!last node error code [int32]
        static const char * const NODE_HEALT_LAST_ERROR_CODE            = "nh_lec";
        //!last node error message [string]
        static const char * const NODE_HEALT_LAST_ERROR_MESSAGE         = "nh_lem";
        //!last node error domain [string]
        static const char * const NODE_HEALT_LAST_ERROR_DOMAIN          = "nh_led";
    }
    /** @} */ // end of NodeHealtDefinitionKey
    
    /** @defgroup NodeHealtDefinitionValue !CHAOS node healt standard value
     *  @{
     */
    //! This namespace enclose all the value for the healt key
    namespace NodeHealtDefinitionValue {
        //! unloaded status
        static const char * const NODE_HEALT_STATUS_UNLOAD      = "Unload";
        //! unloaded status
        static const char * const NODE_HEALT_STATUS_UNLOADING   = "Unloading";
        //! load status
        static const char * const NODE_HEALT_STATUS_LOAD        = "Load";
        //! load status
        static const char * const NODE_HEALT_STATUS_LOADING     = "Loading";
        //! initilizeed status
        static const char * const NODE_HEALT_STATUS_INIT        = "Init";
        //! initilizeed status
        static const char * const NODE_HEALT_STATUS_INITING     = "Initializing";
        //! deinitialized status
        static const char * const NODE_HEALT_STATUS_DEINIT      = "Deinit";
        //! deinitialized status
        static const char * const NODE_HEALT_STATUS_DEINITING   = "Deinitializing";
        //! started status
        static const char * const NODE_HEALT_STATUS_START       = "Start";
        //! started status
        static const char * const NODE_HEALT_STATUS_STARTING    = "Starting";
        //! stopped status
        static const char * const NODE_HEALT_STATUS_STOP        = "Stop";
        //! stopped status
        static const char * const NODE_HEALT_STATUS_STOPING     = "Stoping";
        //! recoverable error status
        static const char * const NODE_HEALT_STATUS_RERROR      = "Recoverable Error";
        //! fatal error status
        static const char * const NODE_HEALT_STATUS_FERROR      = "Fatal Error";
    }
    /** @} */ // end of NodeHealtDefinitionValue
    
    /** @defgroup DataPackKey Chaos Data Pack output attirbute
     This is the collection of the standard key that are contained into the output
     attribute data pack that describe a producer state
     @{
     */
    //! Namespace for standard constant used for output attribute of a producer
    namespace DataPackOutputKey {
        //!this define key associated to the trigger
        static const char * const DPOK_TRIGGER_CODE             = "dpok_trigger_key";
    }
    /** @} */ // end of DataPackKey
    
    /** @defgroup DataPackSystemKey Chaos Data Pack for System Attribute
     @{
     these are the stantdard key for chaos system attirbute
     */
    //! Namespace for standard constant used for system attribute
    namespace DataPackSystemKey {
        
        //!is the last error message occurred into data producer
        static const char * const DP_SYS_UNIT_TYPE			= "dp_sys_unit_type";
        
        //!is the number of run unit
        static const char * const DP_SYS_RUN_UNIT_AVAILABLE	= "dp_sys_ru_available";
        
        //!is the run unit identifier
        static const char * const DP_SYS_RUN_UNIT_ID		= "dp_sys_ru_id";
        
        //!is the busy flag
        static const char * const DP_SYS_QUEUED_CMD         = "dp_sys_que_cmd";
        
        //!is the warning
        static const char * const DP_SYS_STACK_CMD          = "dp_sys_stack_cmd";
    }
    /** @} */ // end of DataPackSystemKey
    
    /** @defgroup DatasetDefinitionkey Dataset definition key
     *  This is the collection of the key for the device dataset
     *  @{
     */
    //! Name space for grupping option used for define the dataset of the device abstraction
    namespace DatasetDefinitionkey {
        //!key for dataset descriptors array {[domain, name, paramteres....]}
        static const char * const DESCRIPTION                       = "ds_desc";
        //!key for dataset timestampt validity
        static const char * const TIMESTAMP                         = "ds_timestamp";
        //!key for the name of dataset attribute
        static const char * const ATTRIBUTE_NAME                    = "ds_attr_name";
        //!key for the tag of the dataset attrbiute
        static const char * const ATTRIBUTE_TAG                     = "ds_attr_tag";
        //!key representing the type of parameter
        static const char * const ATTRIBUTE_TYPE                    = "ds_attr_type";
        //!key for the units ofr the attrbiute(ampere, volts)
        static const char * const ATTRIBUTE_UNIT                    = "ds_attr_unit";
        //!key representing the name of the parameter
        static const char * const ATTRIBUTE_DESCRIPTION             = "ds_attr_desc";
        //!key representig the information for the parameter
        static const char * const ATTRIBUTE_DIRECTION               = "ds_attr_dir";
        //!key representing the value max size where need (type different from rawd data type ex: int32)
        static const char * const VALUE_MAX_SIZE                    = "ds_max_size";
        //!key representing the default value
        static const char * const DEFAULT_VALUE                     = "ds_default_value";
        //!key representing the default value
        static const char * const MAX_RANGE                         = "ds_max_range";
        //!key representing the default value
        static const char * const MIN_RANGE                         = "ds_min_range";
    }
    /** @} */ // end of DatasetDefinitionkey
    
    /** @defgroup DataPackPrefixID Chaos Data Prefix
     This collection is a set for the prefix used for identify the domain
     for the unique id key in chaos data cloud
     @{
     */
    //! Namespace for the domain for the unique identification key
    namespace DataPackPrefixID {
        static const char * const OUTPUT_DATASET_POSTFIX    = "_o";
        static const char * const INPUT_DATASET_POSTFIX     = "_i";
        static const char * const CUSTOM_DATASET_POSTFIX    = "_c";
        static const char * const SYSTEM_DATASET_POSTFIX    = "_s";
        static const char * const DEV_ALARM_DATASET_POSTFIX = "_a";
        static const char * const CU_ALARM_DATASET_POSTFIX  = "_w";
        static const char * const HEALTH_DATASET_POSTFIX    = NodeHealtDefinitionKey::HEALT_KEY_POSTFIX;
    }
    /** @} */ // end of DataPackPrefixID
#define DPCK_LAST_DATASET_INDEX	 6
    /** @} */ // end of DataPackCommonKey
    
    //! return the postfix of the dataset type
    static inline const char * const datasetTypeToPostfix(unsigned int ds_type) {
        switch(ds_type) {
            case DataPackCommonKey::DPCK_DATASET_TYPE_OUTPUT:
                return DataPackPrefixID::OUTPUT_DATASET_POSTFIX;
                //!Integer char bit length
            case DataPackCommonKey::DPCK_DATASET_TYPE_INPUT:
                return DataPackPrefixID::INPUT_DATASET_POSTFIX;
                //!Integer 8 bit length
            case DataPackCommonKey::DPCK_DATASET_TYPE_CUSTOM:
                return DataPackPrefixID::CUSTOM_DATASET_POSTFIX;
                //!Integer 16 bit length
            case DataPackCommonKey::DPCK_DATASET_TYPE_SYSTEM:
                return DataPackPrefixID::SYSTEM_DATASET_POSTFIX;
                
            case DataPackCommonKey::DPCK_DATASET_TYPE_HEALTH:
                return DataPackPrefixID::HEALTH_DATASET_POSTFIX;
                //!Integer 32 bit length
            case DataPackCommonKey::DPCK_DATASET_TYPE_DEV_ALARM:
                return DataPackPrefixID::DEV_ALARM_DATASET_POSTFIX;
                //!Integer 64 bit length
            case DataPackCommonKey::DPCK_DATASET_TYPE_CU_ALARM:
                return DataPackPrefixID::CU_ALARM_DATASET_POSTFIX;
            default:
                return "";
        }
    }
    
    //! return the type of the dataset postfix
    static inline const int datasetTypeToPostfix(const std::string& ds_postfix) {
        if(ds_postfix.compare(DataPackPrefixID::OUTPUT_DATASET_POSTFIX) == 0){return DataPackCommonKey::DPCK_DATASET_TYPE_OUTPUT;}
        if(ds_postfix.compare(DataPackPrefixID::INPUT_DATASET_POSTFIX) == 0){return DataPackCommonKey::DPCK_DATASET_TYPE_INPUT;}
        if(ds_postfix.compare(DataPackPrefixID::CUSTOM_DATASET_POSTFIX) == 0){return DataPackCommonKey::DPCK_DATASET_TYPE_CUSTOM;}
        if(ds_postfix.compare(DataPackPrefixID::SYSTEM_DATASET_POSTFIX) == 0){return DataPackCommonKey::DPCK_DATASET_TYPE_SYSTEM;}
        if(ds_postfix.compare(DataPackPrefixID::DEV_ALARM_DATASET_POSTFIX) == 0){return DataPackCommonKey::DPCK_DATASET_TYPE_DEV_ALARM;}
        if(ds_postfix.compare(DataPackPrefixID::CU_ALARM_DATASET_POSTFIX) == 0){return DataPackCommonKey::DPCK_DATASET_TYPE_CU_ALARM;}
        if(ds_postfix.compare(DataPackPrefixID::HEALTH_DATASET_POSTFIX) == 0){return DataPackCommonKey::DPCK_DATASET_TYPE_HEALTH;}
        return -1;
    }
    
    static inline const char* datasetTypeToHuman(unsigned int domain) {
        switch (domain) {
            case DataPackCommonKey::DPCK_DATASET_TYPE_OUTPUT:
                return "output";
            case DataPackCommonKey::DPCK_DATASET_TYPE_INPUT:
                return "input";
            case DataPackCommonKey::DPCK_DATASET_TYPE_CUSTOM:
                return "custom";
            case DataPackCommonKey::DPCK_DATASET_TYPE_SYSTEM:
                return "system";
            case DataPackCommonKey::DPCK_DATASET_TYPE_HEALTH:
                return "health";
            case DataPackCommonKey::DPCK_DATASET_TYPE_DEV_ALARM:
                return "device_alarms";
            case DataPackCommonKey::DPCK_DATASET_TYPE_CU_ALARM:
                return "cu_alarms";
            default:
                return "unknown";
        }
    }
    static inline unsigned int HumanTodatasetType(const std::string& domain) {
        if(domain == "output")
            return DataPackCommonKey::DPCK_DATASET_TYPE_OUTPUT;
        if(domain=="input")
            return DataPackCommonKey::DPCK_DATASET_TYPE_INPUT;
        if(domain=="custom")
            return DataPackCommonKey::DPCK_DATASET_TYPE_CUSTOM;
        if(domain=="system")
            return DataPackCommonKey::DPCK_DATASET_TYPE_SYSTEM;
        if(domain=="health")
            return DataPackCommonKey::DPCK_DATASET_TYPE_HEALTH;
        if(domain=="device_alarms")
            return DataPackCommonKey::DPCK_DATASET_TYPE_DEV_ALARM;
        if(domain=="cu_alarms")
            return DataPackCommonKey::DPCK_DATASET_TYPE_CU_ALARM;
        return DataPackCommonKey::DPCK_DATASET_TYPE_OUTPUT;
        
    }
    
    /** @defgroup RpcActionDefinitionKey Action Rpc Protocol key
     *  This is the collection of the key used for the intere rpc protocol
     *  @{
     */
    //! Name space for grupping option used for define the custom action to share via RPC !CHAOS system
    namespace RpcActionDefinitionKey {
        //!command manager rpc tag, this is the tag that rpc subsystem must to use to transfer BSON package between chaos node rpc endpoint
        static const char * const CS_CMDM_RPC_TAG                             = "chaos_rpc";
        //!key for action domain descriptors array {[domain, name, paramteres....]}
        static const char * const CS_CMDM_ACTION_DESC                         = "act_desc";
        
        //!key for the array of object that represent the action paramteres
        //!description
        static const char * const CS_CMDM_ACTION_DESC_PARAM                   = "act_desc_param";
        
        //!key representing the name of the parameter
        static const char * const CS_CMDM_ACTION_DESC_PAR_NAME                = "act_desc_par_name";
        
        //!key representig the information for the parameter
        static const char * const CS_CMDM_ACTION_DESC_PAR_INFO                = "act_desc_par_info";
        
        //!key representing the type of parameter
        static const char * const CS_CMDM_ACTION_DESC_PAR_TYPE                = "act_desc_par_type";
        
        //!comamnd description constant
        //!key for action domain definition
        static const char * const CS_CMDM_ACTION_DOMAIN                       = "act_domain";
        
        //!key for action name definition
        static const char * const CS_CMDM_ACTION_NAME                         = "act_name";
        
        //!key for action message
        static const char * const CS_CMDM_ACTION_MESSAGE                      = "act_msg";
        
        //!key for the specify the id of the request(the current message is an asnwer)
        static const char * const CS_CMDM_MESSAGE_ID                          = "act_msg_id";
        
        //!key for action name definition
        static const char * const CS_CMDM_ACTION_DESCRIPTION                  = "act_desc";
        
        //!key action submission result
        //static const char * const CS_CMDM_ACTION_SUBMISSION_RESULT            = "act_sub_result";
        
        //!key action submission error message
        static const char * const CS_CMDM_ACTION_SUBMISSION_ERROR_MESSAGE     = "act_sub_emsg";
        
        //!key action submission error domain
        static const char * const CS_CMDM_ACTION_SUBMISSION_ERROR_DOMAIN      = "act_sub_edom";
        
        //!key action submission error code
        static const char * const CS_CMDM_ACTION_SUBMISSION_ERROR_CODE        = "act_sub_ecode";
        
        //!key action submission error server address
        /*!
         Identify the address of the server that can be reached or that don't have sent the ack
         message
         */
        static const char * const CS_CMDM_ACTION_SUBMISSION_ERROR_SERVER_ADDR = "act_sub_e_srv_addr";
        
        //!key for action sub command description
        static const char * const CS_CMDM_SUB_CMD                             = "sub_cmd";
        
        //!key for the ip where to send the answer
        static const char * const CS_CMDM_ANSWER_HOST_IP                      = "rh_ans_ip";
        //!key for the ip where to send the answer
        static const char * const CS_CMDM_ANSWER_DOMAIN                      = "rh_ans_domain";
        //!key for the ip where to send the answer
        static const char * const CS_CMDM_ANSWER_ACTION                      = "rh_ans_action";
        //!key for the answer, it is needed byt the requester to recognize the answer
        static const char * const CS_CMDM_ANSWER_ID                          = "rh_ans_msg_id";
        //!ker ofr the ip where to send the rpc pack
        static const char * const CS_CMDM_REMOTE_HOST_IP                     = "rh_ip";
    }
    /** @} */ // end of RpcActionDefinitionKey
    
    
    /** @defgroup ChaosSystemDomainAndActionLabel Chaos System Action Label
     *  This is the collection of the label that identify the name of the action defined at system level(doman "system")
     *  These are the action that are  managed by meta data service to permit the base interaction with other node.
     *  Every node need to register itself through mds for interact with other !CHAOS node
     *  @{
     */
    namespace ChaosSystemDomainAndActionLabel {
        //! The chaos action domain for system message
        static const char * const SYSTEM_DOMAIN									= "system";
        
        //! Action to retrive all device id
        static const char * const MDS_GET_ALL_DEVICE							= "getAllActiveDevice";
        
        //! Perform the heart beat of the cu
        static const char * const MDS_CU_HEARTBEAT								= "heartbeatControlUnit";
        
        //! Perform request of the network address for a node identified by a device id
        static const char * const MDS_GET_NODE_ADDRESS							= "getNodeNetworkAddress";
        
        //! This action provide to the shutdown porcess of the enteir daemon
        //! that runt the active contorl units. All it will be gracefull shutten down
        //! before daemon exit
        static const char * const ACTION_SYSTEM_SHUTDOWN                        = "shutdownUnitServer";
        
        //! key for the server alias used by the instance [string]
        static const char * const UNIT_SERVER_STATES_ANSWER                     = "unit_server_states";
        
        //! Action called by mds for ack message in the unit server registration process
        static const char * const ACTION_NODE_REG_ACK                           = "nodeRegistrationACK";
    }
    /** @} */ // end of ChaosSystemDomainAndActionLabel
    
    /** @defgroup PerformanceSystemRpcKey Chaos performance system
     * this is the collection of the rpc key for interacting with
     * internal performance system
     *  @{
     */
    namespace PerformanceSystemRpcKey {
        //-------------------------performance-----------------------
        //! The chaos action domain for system message
        static const char * const SYSTEM_PERFORMANCE_DOMAIN = "system:perf";
        
        static const char * const ACTION_PERFORMANCE_INIT_SESSION= "sp:init_session";
        
        static const char * const ACTION_PERFORMANCE_CLOSE_SESSION= "sp:close_session";
        
        static const char * const KEY_REQUEST_SERVER_DESCRITPION = "sp::req_serv_desc";
    }
    /** @} */ // end of PerformanceSystemRpcKey
    
    namespace common {
        namespace event {
            /** @defgroup EventConfiguration Chaos event constant for server
             and cleint configuration
             @{
             */
            //! Name space for grupping option used for commandline or in config file
            namespace EventConfiguration {
                //!  for choice the implementation
                static const char * const   OPTION_KEY_EVENT_ADAPTER_IMPLEMENTATION     = "evt_adpt_impl";
                //! @Configuration for alert event multicast ip
                static const char * const   CONF_EVENT_ALERT_MADDRESS                   = "239.255.0.1";
                //! @Configuration for instruments event multicast ip
                static const char * const   CONF_EVENT_INSTRUMENT_MADDRESS              = "239.255.0.2";
                //! @Configuration for command event multicast ip
                static const char * const   CONF_EVENT_COMMAND_MADDRESS                 = "239.255.0.3";
                //! @Configuration for custom event multicast ip
                static const char * const   CONF_EVENT_CUSTOM_MADDRESS                  = "239.255.0.4";
                //! @Configuration for event multicast ip port
                static const unsigned short CONF_EVENT_PORT                             = 50000;
            }
            /** @} */ // end of EventConfiguration
        }
    }
    
    /*
     * separator to be used in node naming
     */
    static const char PATH_SEPARATOR                                                ='/';
}
#endif
