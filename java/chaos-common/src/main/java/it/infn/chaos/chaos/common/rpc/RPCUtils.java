/**
 * 
 */
package it.infn.chaos.chaos.common.rpc;

import it.infn.chaos.chaos.common.exception.ChaosException;

import org.bson.BasicBSONObject;

/**
 * @author bisegni
 */
public class RPCUtils {

	/**
	 * @param bson
	 * @param errorDomain
	 * @param errorMsg
	 * @param errCode
	 */
	static public void addRefExceptionElementToBson(BasicBSONObject bson, String errorDomain, String errorMsg, int errCode) {
		if (bson == null)
			return;
		if (errorDomain != null)
			bson.append(RPCConstants.CS_CMDM_ACTION_SUBMISSION_ERROR_MESSAGE, errorDomain);
		if (errorMsg != null)
			bson.append(RPCConstants.CS_CMDM_ACTION_SUBMISSION_ERROR_DOMAIN, errorMsg);
		bson.append(RPCConstants.CS_CMDM_ACTION_SUBMISSION_ERROR_CODE, errCode);
	}
	
	/**
	 * @param bson
	 * @param errorDomain
	 * @param errorMsg
	 * @param errCode
	 */
	static public void addRefExceptionToBson(BasicBSONObject bson, ChaosException exc) {
		addRefExceptionElementToBson(bson, exc.getDomain(), exc.getMessage(), exc.getCode());
	}
}
