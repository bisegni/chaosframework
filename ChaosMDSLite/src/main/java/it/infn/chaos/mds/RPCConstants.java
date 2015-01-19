/**
 * 
 */
package it.infn.chaos.mds;

/**
 * @author bisegni
 */
public class RPCConstants {
	// comamnd description constant
	// key for action domain definition
	public static final String	CS_CMDM_ACTION_DOMAIN						= "act_domain";

	// key for action name definition
	public static final String	CS_CMDM_ACTION_NAME							= "act_name";

	// !key for action message
	public static final String	CS_CMDM_ACTION_MESSAGE						= "act_msg";

	// !key for the specify the id of the request(the current message is an
	// asnwer)
	public static final String	CS_CMDM_MESSAGE_ID							= "act_msg_id";

	// key action submission result
	// public static final String CS_CMDM_ACTION_SUBMISSION_RESULT = "act_sub_result";

	// key action submission error message
	public static final String	CS_CMDM_ACTION_SUBMISSION_ERROR_MESSAGE		= "act_sub_emsg";

	// key action submission error domain
	public static final String	CS_CMDM_ACTION_SUBMISSION_ERROR_DOMAIN		= "act_sub_edom";

	// key action submission error code
	public static final String	CS_CMDM_ACTION_SUBMISSION_ERROR_CODE		= "act_sub_ecode";

	// key
	public static final String	CS_CMDM_REMOTE_HOST_IP						= "rh_ip";

	public static final String	CS_CMDM_ANSWER_DOMAIN						= "rh_ans_domain";
	// !key for the ip where to send the answer
	public static final String	CS_CMDM_ANSWER_ACTION						= "rh_ans_action";
	// key
	public static final String	CS_CMDM_REMOTE_HOST_ANSWER_IP				= "rh_ans_ip";

	// key
	public static final String	CS_CMDM_REMOTE_HOST_ANSWER_MESSAGE_ID		= "rh_ans_msg_id";

	// dataset
	public static final String	DATASET_DEVICE_ID							= "ds_attr_dom";
	public static final String	DATASET_DESCRIPTION							= "ds_desc";
	public static final String	DATASET_TIMESTAMP							= "ds_timestamp";
	public static final String	DATASET_ATTRIBUTE_NAME						= "ds_attr_name";
	public static final String	DATASET_ATTRIBUTE_DESCRIPTION				= "ds_attr_desc";
	public static final String	DATASET_ATTRIBUTE_DIRECTION					= "ds_attr_dir";
	public static final String	DATASET_ATTRIBUTE_TYPE						= "ds_attr_type";
	public static final String	DATASET_ATTRIBUTE_DEFAULT_VALUE				= "ds_default_value";
	public static final String	DATASET_ATTRIBUTE_MAX_RANGE					= "ds_max_range";
	public static final String	DATASET_ATTRIBUTE_MIN_RANGE					= "ds_min_range";
	public static final String	CONTROL_UNIT_INSTANCE						= "cu_instance";
	public static final String	CONTROL_UNIT_INSTANCE_NETWORK_ADDRESS		= "cu_instance_net_address";
	public static final String	CONTROL_UNIT_NAME							= "cu_name";

	public static final String	DEVICE_SCHEDULE_DELAY						= "cu_thr-sch-del";

	public static final String	LIVE_DATASERVER_HOST_PORT					= "ds_server_address";

	// ! this action perform the registration for the unit server service
	public static final String	MDS_REGISTER_UNIT_SERVER					= "registerUnitServer";
	// ! key for the server alias used by the instance [string]
	public static final String	MDS_REGISTER_UNIT_SERVER_ALIAS				= "unit_server_alias";
	// ! key for the server rsa key [string]
	public static final String	MDS_REGISTER_UNIT_SERVER_KEY				= "unit_server_key";
	// ! key for the control unit aliases published by the unit server [array fo string]
	public static final String	MDS_REGISTER_UNIT_SERVER_CONTROL_UNIT_ALIAS	= "unit_server_cu_alias";
	// ! key that idetify the result of unit server registration[int32]
	public static final String	MDS_REGISTER_UNIT_SERVER_RESULT				= "reg_unit_serv_result";

	public static final String UNIT_SERVER_CU_STATES = "unit_server_cu_states";

}
