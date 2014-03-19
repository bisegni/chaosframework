/*
 *	chaos_constants.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
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
#ifndef ChaosFramework_ConstrolSystemConstants_h
#define ChaosFramework_ConstrolSystemConstants_h

#include <chaos/common/batch_command/BatchCommandConstants.h>

//Constant for sandbox
namespace chaos {
    
    /** @defgroup CommonParamOption Common Layer Configuration Option
     *  This is the collection of the parameter for common layer to customize the initial setup
     *  @{
     */
    
    //! Name space for grupping option used for commandline or in config file
    namespace InitOption{
        //!  for print the help
        static const char * const   OPT_HELP                            = "help";
        //!   Specify the implementation to use for rp messaging
        static const char * const   OPT_RPC_IMPLEMENTATION              = "rpc-server-impl";
        //!   Specify the network port where rpc system will publish al the service
        static const char * const   OPT_RPC_SERVER_PORT                 = "rpc-server-port";
        //!  Specify the number of the thread that the rpc ssytem must use to process the request
        static const char * const   OPT_RPC_SERVER_THREAD_NUMBER        = "rpc-server-thread-number";
		//!   Specify the network port where the direct io subsystem publish i's priority channel
        static const char * const   OPT_DIRECT_IO_PRIORITY_SERVER_PORT  = "direct_io-priority-server-port";
		//!   Specify the network port where the direct io subsystem publish i's priority channel
        static const char * const   OPT_DIRECT_IO_SERVICE_SERVER_PORT   = "direct_io-service-server-port";
        //!  Specify the number of the thread that the direct io subsystem must use to process the request
        static const char * const   OPT_DIRECT_IO_SERVER_THREAD_NUMBER  = "direct_io-server-thread-number";
		//!   Specify the implementation to use for the direct io subsystem
        static const char * const   OPT_DIRECT_IO_IMPLEMENTATION		= "direct_io-impl";
        //!  Specify the live data servers address with the type host:port it is a multitoken parameter
        static const char * const   OPT_LIVE_DATA_SERVER_ADDRESS        = "live-data-servers";
        //!  Specify the metadata address for the metadataserver
        static const char * const   OPT_METADATASERVER_ADDRESS          = "metadata-server";
        //!  Specify when the log must be forwarded on console
        static const char * const   OPT_LOG_ON_CONSOLE                  = "log-on-console";
        //!  Specify when the log must be forwarded on file
        static const char * const   OPT_LOG_ON_FILE                     = "log-on-file";
        //!  Specify when the file path of the log
        static const char * const   OPT_LOG_FILE                        = "log-file";
        //!  Specifi the level of the log going
        static const char * const   OPT_LOG_LEVEL                       = "log-level";
        //!  Specify the ip where publish the framework
        static const char * const   OPT_PUBLISHING_IP                   = "publishing-ip";
        //!  Specify the ip where publish the framework
        static const char * const   OPT_DATA_IO_IMPL					= "data-io-impl";
    }
    /** @} */ // end of ParamOption
    
    /** @defgroup RpcConfigurationKey Rpc System Configuraiton
     *  This is the collection of the key for the parameter used to configura rpc system
     *  @{
     */
    //! Name space for grupping option used for rpc system configuration
    namespace RpcConfigurationKey {
        //!  specify the type of the adatpre to be used
        static const char * const CS_CMDM_RPC_ADAPTER_TYPE                = "cs|cmdm|rpc_adapter_type";
        //!  specify the port where the rpc must publish the his socket
        static const char * const CS_CMDM_RPC_ADAPTER_TCP_UDP_PORT        = "cs|cmdm|rpc_adapter_port";
        //!  the number of the thread to user
        static const char * const CS_CMDM_RPC_ADAPTER_THREAD_NUMBER       = "cs|cmdm|rpc_adapter_tn";
    }
    /** @} */ // end of RpcConfigurationKey
    
	namespace common {
		namespace direct_io {
			/** @defgroup DirectIOConfigurationKey DirectIO System Configuraiton
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
				
			}
		}
	}
    /** @} */ // end of DirectIOConfigurationKey
	
    /** @defgroup CUDefinitionKey Control Unit Definition
     *  This is the collection of the key for the control unit definition
     *  @{
     */
    //! Name space for grupping option used for define the control unit
    namespace CUDefinitionKey {
        //! delay beetwen a subseguent cu start method call
        static const char * const CS_CM_THREAD_SCHEDULE_DELAY                   = "cs|cm|cu|thr|sch_del";
        //! start the CU without metadataserver permission
        static const char * const CS_CM_CU_AUTOSTART                            = "cs|cm|cu|autostart";
        //! start the CU without metadataserver presence
        static const char * const CS_CM_CU_AUTOSTART_NO_MS                        = "cs|cm|cu|autostart_no_ms";
        //!key representing the type of parameter
        static const char * const CS_CM_CU_NAME                                 = "cs|cm|cu_name";
        //!key representing the type of parameter
        static const char * const CS_CM_CU_INSTANCE                             = "cs|cm|cu_instance";
        //!Control Unit instance internal address
        static const char * const CS_CM_CU_INSTANCE_NET_ADDRESS                 = "cs|cm|cu_instance_net_address";
        //!key representing the type of parameter
        static const char * const CS_CM_CU_DESCRIPTION                          = "cs|cm|cu_desc";
        //!key representing the type of parameter
        static const char * const CS_CM_CU_CLASS                                = "cs|cm|cu_class";
        //!key representing the type of parameter
        static const char * const CS_CM_CU_UUID                                 = "cs|cm|cu_uuid";
		//!key representing the type of control unit
		/*!
		 \ingroup CUStateKey
		 */
        static const char * const CS_CM_CU_TYPE									= "cs|cm|cu_type";
        
    }
    /** @} */ // end of CUDefinitionKey
    
    /** @} */ // end of RpcConfigurationKey
    
    
    namespace utility {
        
        namespace service_state_machine {
            
            namespace InizializableServiceType {
                typedef enum {
                    IS_DEINTIATED = 0,
                    IS_INITIATED = 1
                } InizializableServiceState;
            }
            
            namespace StartableServiceType {
                typedef enum {
                    SS_STARTED = 2,
                    SS_STOPPED = 3
                } StartableServiceState;
            }
        }
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
			DEINIT  = 0,
			INIT    = 1,
			START   = 2,
            STOP    = 3
        } ControlUnitState;
    }
    /** @} */ // end of CUStateKey
    
	/** @defgroup CUType Control Unit Default Type
     *  This is the collection of the key for the classification of the control unit types
     *  @{
     */
    //! Name space for grupping control unit types
	namespace CUType {
		static const char * const RTCU	= "rtcu";
		static const char * const SCCU  = "sccu";
	}
    /** @} */ // end of CUType
	
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
        //!key for the domain of the attribute dataset
        static const char * const DEVICE_ID                         = "ds_attr_dom";
        //!key for the name of dataset attribute
        static const char * const ATTRIBUTE_NAME                    = "ds_attr_name";
        //!key for the name of dataset attribute
        static const char * const ATTRIBUTE_VALUE                   = "ds_attr_value";
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
    
    /** @defgroup ChaosDataType Chaos Basic datatype
     *  This is the collection of the definition of the chaos basic datatype
     *  @{
     */
    //! Name space for grupping the definition of the chaos basic datatype
    namespace DataType {
        //!typede for datatype
        typedef enum {
            //!Integer 32 bit length
            TYPE_INT32 = 0,
            //!Integer 64 bit length
            TYPE_INT64,
            //!Double 64 bit length
            TYPE_DOUBLE,
            //!C string variable length
            TYPE_STRING,
            //!byte array variable length
            TYPE_BYTEARRAY,
            //!struct type
            TYPE_STRUCT,
            //!bool variable length
            TYPE_BOOLEAN
        } DataType;
        
        //!define the direction of dataset element
        typedef enum {
			Input = 0,
			Output=1,
			Bidirectional=2,
        } DataSetAttributeIOAttribute;
    }
    
    /** @} */ // end of DatasetDefinitionkey
    
    /** @defgroup ChaosErrorCode Chaos Error Code
     *  This is the collection of the definition of the chaos error code
     *  @{
     */
    //! Name space for grupping the definition of the chaos error code
    namespace ErrorCode {
        //!typede for datatype
        typedef enum {
            //!Integer 32 bit length
            EC_NO_ERROR = 0,
            EC_TIMEOUT = 1,
            EC_ATTRIBUTE_NOT_FOUND,
            EC_ATTRIBUTE_BAD_DIR,
            EC_ATTRIBUTE_TYPE_NOT_SUPPORTED
        } ErrorCode;
    }
    /** @} */ // end of ChaosDataType
    
    /** @} */ // end of ChaosDataType
    
    /** @defgroup RpcActionDefinitionKey Action Rpc Sharing Definition key
     *  This is the collection of the key for the Action Rpc Sharing Definition key
     *  @{
     */
    //! Name space for grupping option used for define the custom action to share via RPC chaos system
    namespace RpcActionDefinitionKey {
        //!command manager rpc tag, this is the tag that rpc subsystem must to use to transfer BSON package between chaos node rpc endpoint
        static const char * const CS_CMDM_RPC_TAG                                = "chaos_rpc";
        //!key for action domain descriptors array {[domain, name, paramteres....]}
        static const char * const CS_CMDM_ACTION_DESC                         = "cs|cmdm|act_desc";
        
        //!key for the array of object that represent the action paramteres
        //!description
        static const char * const CS_CMDM_ACTION_DESC_PARAM                   = "cs|cmdm|act_desc_param";
        
        //!key representing the name of the parameter
        static const char * const CS_CMDM_ACTION_DESC_PAR_NAME                = "cs|cmdm|act_desc_par_name";
        
        //!key representig the information for the parameter
        static const char * const CS_CMDM_ACTION_DESC_PAR_INFO                = "cs|cmdm|act_desc_par_info";
        
        //!key representing the type of parameter
        static const char * const CS_CMDM_ACTION_DESC_PAR_TYPE                = "cs|cmdm|act_desc_par_type";
        
        //!comamnd description constant
        //!key for action domain definition
        static const char * const CS_CMDM_ACTION_DOMAIN                       = "cs|cmdm|act_domain";
        
        //!key for action name definition
        static const char * const CS_CMDM_ACTION_NAME                         = "cs|cmdm|act_name";
        
        //!key for action message
        static const char * const CS_CMDM_ACTION_MESSAGE                      = "cs|cmdm|act_msg";
        
        //!key for the specify the id of the request(the current message is an asnwer)
        static const char * const CS_CMDM_MESSAGE_ID                          = "cs|cmdm|act_msg_id";
        
        //!key for action name definition
        static const char * const CS_CMDM_ACTION_DESCRIPTION                  = "cs|cmdm|act_desc";
        
        
        //!key action submission result
        //static const char * const CS_CMDM_ACTION_SUBMISSION_RESULT            = "cs|cmdm|act_sub_result";
        
        //!key action submission error message
        static const char * const CS_CMDM_ACTION_SUBMISSION_ERROR_MESSAGE     = "cs|cmdm|act_sub_emsg";
        
        //!key action submission error domain
        static const char * const CS_CMDM_ACTION_SUBMISSION_ERROR_DOMAIN      = "cs|cmdm|act_sub_edom";
        
        //!key action submission error code
        static const char * const CS_CMDM_ACTION_SUBMISSION_ERROR_CODE        = "cs|cmdm|act_sub_ecode";
        
        //!key for action sub command description
        static const char * const CS_CMDM_SUB_CMD                             = "cs|cmdm|sub_cmd";
        
        //!key for the ip where to send the answer
        static const char * const CS_CMDM_ANSWER_HOST_IP                      = "cs|cmdm|rh_ans_ip";
        //!key for the ip where to send the answer
        static const char * const CS_CMDM_ANSWER_DOMAIN                      = "cs|cmdm|rh_ans_domain";
        //!key for the ip where to send the answer
        static const char * const CS_CMDM_ANSWER_ACTION                      = "cs|cmdm|rh_ans_action";
        //!key for the answer, it is needed byt the requester to recognize the answer
        static const char * const CS_CMDM_ANSWER_ID                          = "cs|cmdm|rh_ans_msg_id";
        //!ker ofr the ip where to send the rpc pack
        static const char * const CS_CMDM_REMOTE_HOST_IP                     = "cs|cmdm|rh_ip";
    }
    /** @} */ // end of RpcActionDefinitionKey
    
    
    /** @defgroup ChaosSystemDomainAndActionLabel Chaos System Action Label
     *  This is the collection of the label that identify the name of the action defined at system level(doman "system")
     *  These are the action that are used by chaos to interact with standard node(CUToolkti, UIToolkit, Metadata Server)
     *  @{
     */
    namespace ChaosSystemDomainAndActionLabel {
        //! The chaos action domain for system message
        static const char * const SYSTEM_DOMAIN             = "system";
        //! This action provide to the shutdown porcess of the enteir daemon
        //! that runt the active contorl units. All it will be gracefull shutten down
        //! before daemon exit
        static const char * const ACTION_SYSTEM_SHUTDOWN    = "shutdown";
        //! Start the control unit intialization, the action need the default value
        //! of the input attribute for a determinate device
        static const char * const ACTION_DEVICE_INIT        = "initDevice";
        //! Deinitialization of a control unit, if it is in run, the stop phase
        //! is started befor deinitialization one
        static const char * const ACTION_DEVICE_DEINIT      = "deinitDevice";
        //! start the run method schedule for a determinated device
        static const char * const ACTION_DEVICE_START       = "startDevice";
        //! pause the run method for a determinated device
        static const char * const ACTION_DEVICE_STOP        = "stopDevice";
        //! return the state of the device
        static const char * const ACTION_DEVICE_GET_STATE   = "getState";
		//! return the control unit information
        static const char * const ACTION_CU_GET_INFO		= "getInfo";

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
		
		static const char * const KEY_REQUEST_SERVER_DESCRITPION = "sp::req_serv_desc";
	}
	/** @} */ // end of PerformanceSystemRpcKey
	
    /** @defgroup DataProxyConfigurationKey Chaos data proxy configuration
     *  This is the collection of the key used to configure the DataProxy server
	 *  address and port and how the client's(cu and ui) need to access to it (round robin or fail over)
     *  @{
     */
    //! This is the collection of the key to configura history and live channel
    namespace DataProxyConfigurationKey {
        static const char * const CS_LIB_METADATASET_ADDRESS					="mds_network_addr";
        //!root key for DataManager Configuration
        static const char * const CS_DM_CONFIGURATION							= "cs|dm|configuration";
        //!the numebr of the thread ofr the output buffer
        static const char * const CS_DM_OUTPUT_BUFFER_THREAD_NUM				= "cs|dm|out_buf_thread_num";
        //!the numebr of the millisecond to wait untile the next
        //!data will be send to the live buffer
        static const char * const CS_DM_OUTPUT_BUFFER_LIVE_TIME					= "cs|dm|out_buf_live_time";
        //!the numebr of the microsecond to wait untile the next
        //!data will be send to the hst buffer
        static const char * const CS_DM_OUTPUT_BUFFER_HST_TIME					= "cs|dm|out_buf_hst_time";
        //!live data address
        static const char * const CS_DM_LD_SERVER_ADDRESS						= "cs|dm|ld|server_address";
        //!key associated with the device identification in a k/v storage ien
        static const char * const CS_DM_LD_DEVICE_ADDRESS_KEY                   = "ld.device_addr_key";
		//!key associated with the device identification in a k/v storage ien
        static const char * const DATAPROXY_CLIENT_CONNECTION_MODE				= "chaos:dp:conn_mode";
    }
    /** @} */ // end of DataProxyConfigurationKey
    
    
    /** @defgroup ChaosDataPackKey Chaos Data Pack standard for hardware instrument
     This is the collection of the standard key that are contained into the data pack
     that describe a device hardware metrix on it's input and output channel
     @{
     */
    //! Namespace for standard constant used for hardware instrument
    namespace DataPackKey{
        //!define the device id key, this represent the
        //!primary key of the device
        static const char * const CS_CSV_DEVICE_ID                            = "cs|csv|device_id";
        
        //!this define the timestamp of the data rapresented
        //!in the dataset row
        static const char * const CS_CSV_TIME_STAMP                           = "cs|csv|timestamp";
        
        //!this define key associated to the trigger
        static const char * const CS_CSV_TRIGGER_KEY                          = "cs|csv|trigger_key";
    }
    /** @} */ // end of ChaosLiveHistoryConfiguration
    
    namespace event {
        /** @defgroup EventConfiguration Chaos event constant for server
         and cleint configuration
         @{
         */
        //! Name space for grupping option used for commandline or in config file
        namespace EventConfiguration {
            //!  for choice the implementation
            static const char * const   OPTION_KEY_EVENT_ADAPTER_IMPLEMENTATION     = "evt_adpt_impl";
            //! @Configuraiton for alert event multicast ip
            static const char * const   CONF_EVENT_ALERT_MADDRESS                   = "239.255.0.1";
            //! @Configuraiton for instruments event multicast ip
            static const char * const   CONF_EVENT_INSTRUMENT_MADDRESS              = "239.255.0.2";
            //! @Configuraiton for command event multicast ip
            static const char * const   CONF_EVENT_COMMAND_MADDRESS                 = "239.255.0.3";
            //! @Configuraiton for custom event multicast ip
            static const char * const   CONF_EVENT_CUSTOM_MADDRESS                  = "239.255.0.4";
            //! @Configuraiton for event multicast ip port
            static const unsigned short CONF_EVENT_PORT                             = 5000;
        }
        /** @} */ // end of EventConfiguration
    }
}
#endif
