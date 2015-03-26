package it.infn.chaos.mds.rpc.server;

import it.infn.chaos.mds.RPCConstants;
import it.infn.chaos.mds.SingletonServices;

import org.bson.BasicBSONObject;
import org.ref.common.exception.RefException;

public class ActionHandlerExecutionUnit implements Runnable {
	public RPCActionHadler	handler	= null;
	public BasicBSONObject	data	= null;

	/**
	 * @param h
	 * @param d
	 */
	public ActionHandlerExecutionUnit(RPCActionHadler h, BasicBSONObject d) {
		handler = h;
		data = d;
	}

	@Override
	public void run() {
		BasicBSONObject resultMessage = new BasicBSONObject();
		BasicBSONObject resultPack = new BasicBSONObject();

		if (handler == null || data == null)
			return;

		String domain = data.getString(RPCConstants.CS_CMDM_ACTION_DOMAIN);
		String action = data.getString(RPCConstants.CS_CMDM_ACTION_NAME);
		if (domain == null && action == null)
			return;
		BasicBSONObject actionMessage = (BasicBSONObject) (data.containsField(RPCConstants.CS_CMDM_ACTION_MESSAGE) ? data.get(RPCConstants.CS_CMDM_ACTION_MESSAGE) : null);
		// get info for replay
		try {

			actionMessage = handler._handleAction(domain, action, actionMessage);
			resultPack.append(RPCConstants.CS_CMDM_ACTION_SUBMISSION_ERROR_CODE, 0);
			if (actionMessage != null)
				resultPack.append(RPCConstants.CS_CMDM_ACTION_MESSAGE, actionMessage);
		} catch (RefException e) {
			RPCUtils.addRefExceptionToBson(resultPack, e);
		}

		String responseAddress = data.containsField(RPCConstants.CS_CMDM_REMOTE_HOST_ANSWER_IP) ? data.getString(RPCConstants.CS_CMDM_REMOTE_HOST_ANSWER_IP) : null;
		String reponseDomain = data.containsField(RPCConstants.CS_CMDM_ANSWER_DOMAIN) ? data.getString(RPCConstants.CS_CMDM_ANSWER_DOMAIN) : null;
		String reponseAction = data.containsField(RPCConstants.CS_CMDM_ANSWER_ACTION) ? data.getString(RPCConstants.CS_CMDM_ANSWER_ACTION) : null;
		Integer responseID = data.containsField(RPCConstants.CS_CMDM_REMOTE_HOST_ANSWER_MESSAGE_ID) ? data.getInt(RPCConstants.CS_CMDM_REMOTE_HOST_ANSWER_MESSAGE_ID) : null;

		if (responseAddress == null || reponseDomain == null)
			return;

		resultMessage.append(RPCConstants.CS_CMDM_REMOTE_HOST_IP, responseAddress);
		resultMessage.append(RPCConstants.CS_CMDM_ACTION_DOMAIN, reponseDomain);

		if (reponseAction != null) {
			resultMessage.append(RPCConstants.CS_CMDM_ACTION_NAME, reponseAction);
		}

		if (responseID != null) {
			resultPack.append(RPCConstants.CS_CMDM_MESSAGE_ID, responseID);
		}

		// add the action message
		resultMessage.put(RPCConstants.CS_CMDM_ACTION_MESSAGE, resultPack);
		try {
			//System.out.println(resultMessage.toString());
			SingletonServices.getInstance().getMdsRpcClient().sendMessage(resultMessage);
		} catch (Throwable e) {
		
		}
		
		//System.out.println("Done executing ActionHandlerExecutionUnit");
	}
}
