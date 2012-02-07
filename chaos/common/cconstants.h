    //
    //  ConstolSystemConstants.h
    //  ChaosFramework
    //
    //  Created by Claudio Bisegni on 21/06/11.
    //  Copyright 2011 INFN. All rights reserved.
    //

#ifndef ChaosFramework_ConstrolSystemConstants_h
#define ChaosFramework_ConstrolSystemConstants_h

    //Constant for sandbox
namespace chaos {
    
    /** @defgroup CommonParamOption Common Layer Configuration Option
     *  This is the collection of the parameter for common layer to customize the initial setup
     *  @{
     */
    
    //! Name space for grupping option used for commandline or in config file
    namespace UserOption{
            //! @Option for print the help
        static const char * const   OPT_HELP                          = "help";
            //!  @Option Specify the network port where rpc system will publish al the service
        static const char * const   OPT_RPC_SERVER_PORT              ="rpc-server-port";
            //! @Option Specify the number of the thread that the rpc ssytem must use to process the request 
        static const char * const   OPT_RPC_SERVER_THREAD_NUMBER     ="rpc-server-thread-number";
            //! @Option Specify the live data servers address with the type host:port it is a multitoken parameter 
        static const char * const   OPT_LIVE_DATA_SERVER_ADDRESS     ="live-data-servers";
            //! @Option Specify the metadata address for the metadataserver
        static const char * const   OPT_METADATASERVER_ADDRESS       ="metadata-server";
            //! @Option Specify when the log must be forwarded on console
        static const char * const   OPT_LOG_ON_CONSOLE               ="log-on-console";
            //! @Option Specify when the log must be forwarded on file
        static const char * const   OPT_LOG_ON_FILE                  ="log-on-file";
            //! @Option Specify when the file path of the log
        static const char * const   OPT_LOG_FILE                     ="log-file";
    }
    /** @} */ // end of ParamOption
    
    /** @defgroup RpcConfigurationKey Rpc System Configuraiton
     *  This is the collection of the key for the parameter used to configura rpc system
     *  @{
     */
     //! Name space for grupping option used for rpc system configuration
    namespace RpcConfigurationKey {
        //! @Option specify the type of the adatpre to be used
        static const char * const CS_CMDM_RPC_ADAPTER_TYPE                = "cs|cmdm|rpc_adapter_type";
        //! @Option specify the port where the rpc must publish the his socket
        static const char * const CS_CMDM_RPC_ADAPTER_TCP_UDP_PORT        = "cs|cmdm|rpc_adapter_port";
        //! @Option the number of the thread to user
        static const char * const CS_CMDM_RPC_ADAPTER_THREAD_NUMBER       = "cs|cmdm|rpc_adapter_tn";
    }
    /** @} */ // end of RpcConfigurationKey

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

    }
    /** @} */ // end of CUDefinitionKey

    /** @defgroup DatasetDefinitionkey Dataset definition key
     *  This is the collection of the key for the device dataset
     *  @{
     */
        //! Name space for grupping option used for define the dataset of the device abstraction
    namespace DatasetDefinitionkey {
            //!key for dataset descriptors array {[domain, name, paramteres....]}
        static const char * const CS_CM_DATASET_DESCRIPTION                     = "cs|cm|ds_desc";
        
            //!key for the domain of the attribute dataset
        static const char * const CS_CM_DATASET_DEVICE_ID                       = "cs|cm|ds_attr_dom";
        
            //!key for the name of dataset attribute
        static const char * const CS_CM_DATASET_ATTRIBUTE_NAME                  = "cs|cm|ds_attr_name";
        
            //!key for the tag of the dataset attrbiute
        static const char * const CS_CM_DATASET_ATTRIBUTE_TAG                   = "cs|cm|ds_attr_tag";
        
            //!key representing the type of parameter
        static const char * const CS_CM_DATASET_ATTRIBUTE_TYPE                  = "cs|cm|ds_attr_type";
        
            //!key for the units ofr the attrbiute(ampere, volts)
        static const char * const CS_CM_DATASET_ATTRIBUTE_UNIT                  = "cs|cm|ds_attr_unit";
        
            //!key representing the name of the parameter
        static const char * const CS_CM_DATASET_ATTRIBUTE_DESCRIPTION           = "cs|cm|ds_attr_desc";
        
            //!key representig the information for the parameter
        static const char * const CS_CM_DATASET_ATTRIBUTE_DIRECTION             = "cs|cm|ds_attr_dir";
        
            //!key representing the default value 
        static const char * const CS_CMDM_ACTION_DESC_DEFAULT_VALUE             = "cs|cmdm|act_desc_par_default_value";
            //!key representing the default value 
        static const char * const CS_CMDM_ACTION_DESC_MAX_RANGE                 = "cs|cmdm|act_desc_par_max_range";            
            //!key representing the default value 
        static const char * const CS_CMDM_ACTION_DESC_MIN_RANGE                 = "cs|cmdm|act_desc_par_min_range";
        
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
                TYPE_BYTEARRAY
            } DataType;
        
        //!define the direction of dataset element
        typedef enum {
			Input = 0,
			Output,
			Bidirectional,
        } DataSetAttributeIOAttribute;
    } 
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
        static const char * const CS_CMDM_ACTION_SUBMISSION_RESULT            = "cs|cmdm|act_sub_result";
        
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
        static const char * const CS_CMDM_REMOTE_HOST_IP                        = "cs|cmdm|rh_ip";     
    }
    /** @} */ // end of RpcActionDefinitionKey
    
    
    /** @defgroup ChaosSystemDomainAndActionLabel Chaos System Action Label
     *  This is the collection of the label that identify the name of the action defined at system level(doman "system")
     *  These are the action that are used by chaos to interact with standard node(CUToolkti, UIToolkit, Metadata Server)
     *  @{
     */
    namespace ChaosSystemDomainAndActionLabel {
            //! The chaos action domain for system message
        static const char * const SYSTEM_DOMAIN         = "system";
            //! This action provide to the shutdown porcess of the enteir daemon 
            //! that runt the active contorl units. All it will be gracefull shutten down
            //! before daemon exit
        static const char * const ACTION_CU_SHUTDOWN    = "shutdown";
            //! Start the control unit intialization, the action need the default value
            //! of the input attribute for a determinate device
        static const char * const ACTION_CU_INIT        = "initControlUnit";
            //! Deinitialization of a control unit, if it is in run, the stop phase
            //! is started befor deinitialization one
        static const char * const ACTION_CU_DEINIT      = "deinitControlUnit";
            //! start the run method schedule for a determinated device
        static const char * const ACTION_CU_START       = "startControlUnit";
            //! pause the run method for a determinated device
        static const char * const ACTION_CU_STOP        = "stopControlUnit";
    }
     /** @} */ // end of ChaosSystemDomainAndActionLabel
    
    /** @defgroup LiveHistoryMDSConfiguration Chaos Live and History configuration
     *  This is the collection of the key to configura history and live channel
     *  @{
     */
        //! This is the collection of the key to configura history and live channel
    namespace LiveHistoryMDSConfiguration {
        static const char * const CS_LIB_METADATASET_ADDRESS            ="mds_network_addr";
            //!root key for DataManager Configuration
        static const char * const CS_DM_CONFIGURATION                         = "cs|dm|configuration";
            //!the numebr of the thread ofr the output buffer
        static const char * const CS_DM_OUTPUT_BUFFER_THREAD_NUM              = "cs|dm|out_buf_thread_num";
            //!the numebr of the millisecond to wait untile the next
            //!data will be send to the live buffer
        static const char * const CS_DM_OUTPUT_BUFFER_LIVE_TIME              = "cs|dm|out_buf_live_time";
            //!the numebr of the microsecond to wait untile the next
            //!data will be send to the hst buffer
        static const char * const CS_DM_OUTPUT_BUFFER_HST_TIME               = "cs|dm|out_buf_hst_time";
            //!live data address
        static const char * const CS_DM_LD_SERVER_ADDRESS                 = "cs|dm|ld|server_address";
            //!history data address
        static const char * const CS_DM_HD_SERVER_ADDRESS                 = "cs|dm|hd|server_address";

    } 
    /** @} */ // end of LiveHistoryMDSConfiguration
    

    /** @defgroup ChaosDataPackKey Chaos Data Pack standard key
     *  This is the collection of the standard key of the chaos pack that contain hardware data
     *  @{
     */
        //! This is the collection of the standard key of the chaos pack that contain hardware data
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

    
}

#endif
