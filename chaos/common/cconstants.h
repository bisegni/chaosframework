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
    
        //--------------global values-------------------
    static const char * const CS_LIB_METADATASET_ADDRESS                    = "mds_address";
    static const char * const CS_LIB_INSTANCE_NETWORK_ADDRESS               = "inst_net_address";
        //-------------system action---------------------
        //this constant define the domain name used to publish the action for
        //system layer(all the manager data, control and command)
    static const char * const CS_LIB_ACTION_DOMAIN_NAME                     = "system";
    
    static const char * const CS_LIB_ACTION_DOMAIN_SHUTDOWN                 = "shutdown";
    static const char * const CS_LIB_ACTION_DOMAIN_INIT_CU                  = "initControlUnit";
    static const char * const CS_LIB_ACTION_DOMAIN_DEINIT_CU                = "deinitControlUnit";
    static const char * const CS_LIB_ACTION_DOMAIN_START_CU                 = "startControlUnit";
    static const char * const CS_LIB_ACTION_DOMAIN_STOP_CU                  = "stopControlUnit";
        //-------------system action---------------------
    
        //Control Manager Configuration Constats
    namespace ControlManagerConstant{
        
            //sandbox configuration constants
        namespace ControlUnitSandBoxConstant{
                //delay beetwen a subseguent cu start method call
            static const char * const CS_CM_THREAD_SCHEDULE_DELAY                   = "cs|cm|cu|thr|sch_del";
            
                //list of device id managed by Control Unit
            static const char * const CS_CM_CU_MANAGED_DEVICE_ID              = "cs|cm|cu|managed_dev_id";
        }
        
            //key for the 
        static const char * const CS_CM_CU_DEVICE_NAME                         = "cs|cm|cu|device_name";

            //stast the CU without metadataserver permission 
        static const char * const CS_CM_CU_AUTOSTART                        = "cs|cm|cu|autostart";
        
            //key for dataset descriptors array {[domain, name, paramteres....]}
        static const char * const CS_CM_DATASET_DESC                         = "cs|cm|ds_desc";

            //key for the domain of the attribute dataset
        static const char * const CS_CM_DATASET_ATTRIBUTE_DOMAIN                 = "cs|cm|ds_attr_dom";
        
            //key for the name of dataset attribute
        static const char * const CS_CM_DATASET_ATTRIBUTE_NAME                 = "cs|cm|ds_attr_name";
            
            //key for the tag of the dataset attrbiute
        static const char * const CS_CM_DATASET_ATTRIBUTE_TAG                  = "cs|cm|ds_attr_tag";
        
            //key representing the name of the parameter
        static const char * const CS_CM_DATASET_ATTRIBUTE_DESCRIPTION          = "cs|cm|ds_attr_desc";
        
            //key representig the information for the parameter
        static const char * const CS_CM_DATASET_ATTRIBUTE_DIRECTION            = "cs|cm|ds_attr_dir";
        
            //key representing the type of parameter
        static const char * const CS_CM_DATASET_ATTRIBUTE_TYPE                 = "cs|cm|ds_attr_type";
        
            //key representing the default value 
        static const char * const CS_CMDM_ACTION_DESC_DEFAULT_VALUE            = "cs|cmdm|act_desc_par_default_value";
            //key representing the default value 
        static const char * const CS_CMDM_ACTION_DESC_MAX_RANGE                = "cs|cmdm|act_desc_par_max_range";            
            //key representing the default value 
        static const char * const CS_CMDM_ACTION_DESC_MIN_RANGE                = "cs|cmdm|act_desc_par_min_range";
        
            //key representing the type of parameter
        static const char * const CS_CM_CU_NAME                                 = "cs|cm|cu_name";
            //key representing the type of parameter
        static const char * const CS_CM_CU_INSTANCE                             = "cs|cm|cu_instance";
            //cu instance network address
        static const char * const CS_CM_CU_INSTANCE_NET_ADDRESS                 = "cs|cm|cu_instance_net_address";
            //key representing the type of parameter
        static const char * const CS_CM_CU_DESCRIPTION                          = "cs|cm|cu_desc";
            //key representing the type of parameter
        static const char * const CS_CM_CU_CLASS                                = "cs|cm|cu_class";
            //key representing the type of parameter
        static const char * const CS_CM_CU_UUID                                 = "cs|cm|cu_uuid";    
    }
    
        //Command Manager Constant
    namespace CommandManagerConstant{
            //command manager rpc tag, this is the tag that rpc subsystem must to use
            //to transfer BSON package
        static const char * const CS_CMDM_RPC_TAG                                = "chaos_rpc";
            //rpc adapter constant
        namespace RpcAdapterConstant{
                //root constant
            static const char * const CS_CMDM_RPC_ADAPTER                     = "cs|cmdm|rpc_adapter";
            static const char * const CS_CMDM_RPC_ADAPTER_TYPE                = "cs|cmdm|rpc_adapter_type";
            static const char * const CS_CMDM_RPC_ADAPTER_TCP_UDP_PORT        = "cs|cmdm|rpc_adapter_port";
            static const char * const CS_CMDM_RPC_ADAPTER_THREAD_NUMBER       = "cs|cmdm|rpc_adapter_tn";
        }

            //key for action domain descriptors array {[domain, name, paramteres....]}
        static const char * const CS_CMDM_ACTION_DESC                         = "cs|cmdm|act_desc";
            
            //key for the array of object that represent the action paramteres 
            //description
        static const char * const CS_CMDM_ACTION_DESC_PARAM                   = "cs|cmdm|act_desc_param";
            
            //key representing the name of the parameter
        static const char * const CS_CMDM_ACTION_DESC_PAR_NAME                = "cs|cmdm|act_desc_par_name";
        
            //key representig the information for the parameter
        static const char * const CS_CMDM_ACTION_DESC_PAR_INFO                = "cs|cmdm|act_desc_par_info";
        
            //key representing the type of parameter
        static const char * const CS_CMDM_ACTION_DESC_PAR_TYPE                = "cs|cmdm|act_desc_par_type";
        
            //typede for action param type enum
        typedef enum {
            CS_CMDM_ACTION_DESC_PAR_TYPE_INT32 = 0,
            CS_CMDM_ACTION_DESC_PAR_TYPE_INT64,
            CS_CMDM_ACTION_DESC_PAR_TYPE_DOUBLE,
            CS_CMDM_ACTION_DESC_PAR_TYPE_STRING,
            CS_CMDM_ACTION_DESC_PAR_TYPE_BYTEARRAY
            
        } ActionParamType;
        
            //comamnd description constant
            //key for action domain definition
        static const char * const CS_CMDM_ACTION_DOMAIN                       = "cs|cmdm|act_domain";
        
            //key for action name definition
        static const char * const CS_CMDM_ACTION_NAME                         = "cs|cmdm|act_name";

            //key for action name definition
        static const char * const CS_CMDM_ACTION_DESCRIPTION                  = "cs|cmdm|act_desc";

        
            //key action submission result
        static const char * const CS_CMDM_ACTION_SUBMISSION_RESULT            = "cs|cmdm|act_sub_result";
        
            //key action submission error message
        static const char * const CS_CMDM_ACTION_SUBMISSION_ERROR_MESSAGE     = "cs|cmdm|act_sub_emsg";

            //key action submission error domain
        static const char * const CS_CMDM_ACTION_SUBMISSION_ERROR_DOMAIN      = "cs|cmdm|act_sub_edom";

            //key action submission error code
        static const char * const CS_CMDM_ACTION_SUBMISSION_ERROR_CODE        = "cs|cmdm|act_sub_ecode";

            //key for action sub command description
        static const char * const CS_CMDM_SUB_CMD                             = "cs|cmdm|sub_cmd";
        
            //key for response procedure: if a action is submitted with these two
            //key. a response need to be sent to the ip that has submitte the request
        static const char * const CS_CMDM_REMOTE_HOST_RESPONSE_ID            = "cs|cmdm|rh_resp_id"; 
        static const char * const CS_CMDM_REMOTE_HOST_IP                     = "cs|cmdm|rh_ip"; 
    }    
    
        //data manager constant
    namespace DataManagerConstant{
            //root key for DataManager Configuration
        static const char * const CS_DM_CONFIGURATION                         = "cs|dm|configuration";
            //the numebr of the thread ofr the output buffer
        static const char * const CS_DM_OUTPUT_BUFFER_THREAD_NUM              = "cs|dm|out_buf_thread_num";
            //the numebr of the millisecond to wait untile the next
            //data will be send to the live buffer
        static const char * const CS_DM_OUTPUT_BUFFER_LIVE_TIME              = "cs|dm|out_buf_live_time";
            //the numebr of the microsecond to wait untile the next
            //data will be send to the hst buffer
        static const char * const CS_DM_OUTPUT_BUFFER_HST_TIME              = "cs|dm|out_buf_hst_time";
            //live data constant
        namespace LiveDataConstant{
                //live data address
            static const char * const CS_DM_LD_SERVER_ADDRESS                 = "cs|dm|ld|server_address";
        }
        
        namespace HstDataConstant{
                //live data address
            static const char * const CS_DM_HD_SERVER_ADDRESS                 = "cs|dm|hd|server_address";
        }
        
    }    
    
        //Control System value constant
    namespace ControlSystemValueConstant{
            //define the device id key, this represent the
            //primary key of the device
        static const char * const CS_CSV_DEVICE_ID                            = "cs|csv|device_id";
        
            //this define the timestamp of the data rapresented
            //in the dataset row
        static const char * const CS_CSV_TIME_STAMP                           = "cs|csv|timestamp";
        
            //this define key associated to the trigger
        static const char * const CS_CSV_TRIGGER_KEY                          = "cs|csv|trigger_key";
    }
    
}

#endif
