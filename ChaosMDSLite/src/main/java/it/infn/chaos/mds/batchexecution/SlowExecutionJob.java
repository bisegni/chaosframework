package it.infn.chaos.mds.batchexecution;

import it.infn.chaos.mds.RPCConstants;
import it.infn.chaos.mds.SingletonServices;
import it.infn.chaos.mds.rpc.server.RPCActionHadler;
import it.infn.chaos.mds.rpc.server.RPCClient;

import java.util.Hashtable;
import java.util.UUID;
import java.util.concurrent.Semaphore;
import java.util.concurrent.atomic.AtomicInteger;

import org.bson.BasicBSONObject;
import org.ref.common.exception.RefException;

abstract public class SlowExecutionJob extends RPCActionHadler implements Runnable {
	private AtomicInteger						counterRequestID	= new AtomicInteger();
	private Object								inputData			= null;
	private RPCClient							clientInstance		= null;
	private String								jobDomain			= UUID.randomUUID().toString().substring(0, 10);
	private final Semaphore						available			= new Semaphore(0);
	private Hashtable<Integer, BasicBSONObject>	resultHashTable		= new Hashtable<Integer, BasicBSONObject>();

	public SlowExecutionJob() {
		clientInstance = SingletonServices.getInstance().getMdsRpcClient();

		try {
			registerDomanAndNameForHandler(SingletonServices.getInstance().getMdsRpcServer());
		} catch (RefException e) {
			e.printStackTrace();
		}

	}

	@Override
	public void run() {
		try {
			executeJob();
		} catch (Throwable e1) {
			e1.printStackTrace();
		}
		try {
			deregisterDomanAndNameForHandler(SingletonServices.getInstance().getMdsRpcServer());
		} catch (RefException e) {
			e.printStackTrace();
		}
		System.out.println(this.getClass().getName() + " is closing");
	}

	private void consumeAnswer(BasicBSONObject actionData) {
		if (actionData.containsKey((Object) RPCConstants.CS_CMDM_MESSAGE_ID)) {
			synchronized (resultHashTable) {
				int resultID = actionData.getInt(RPCConstants.CS_CMDM_MESSAGE_ID);
				resultHashTable.put(new Integer(resultID), actionData);
			}
		}

	}

	protected BasicBSONObject sendRequest(String ip_port, String domain, String action, BasicBSONObject requestData, Integer requestID, boolean wait) throws Throwable {
		BasicBSONObject request = new BasicBSONObject();
		request.append(RPCConstants.CS_CMDM_REMOTE_HOST_IP, ip_port);
		request.append(RPCConstants.CS_CMDM_ACTION_DOMAIN, domain);
		request.append(RPCConstants.CS_CMDM_ACTION_NAME, action);
		if (requestData != null) {
			request.append(RPCConstants.CS_CMDM_ACTION_MESSAGE, requestData);
		}

		// add current server as
		int resultID = counterRequestID.incrementAndGet();
		request.append(RPCConstants.CS_CMDM_REMOTE_HOST_ANSWER_IP, "localhost:5500");
		request.append(RPCConstants.CS_CMDM_REMOTE_HOST_ANSWER_MESSAGE_ID, resultID);
		request.append(RPCConstants.CS_CMDM_ANSWER_DOMAIN, jobDomain);
		request.append(RPCConstants.CS_CMDM_ANSWER_ACTION, "response");

		getClientInstance().sendMessage(request);
		if (!wait)
			return null;
		available.acquire();
		synchronized (resultHashTable) {
			return resultHashTable.get(new Integer(resultID));
		}
	}

	protected void sendMessage(String ip_port, String domain, String action, BasicBSONObject requestData) throws Throwable {
		BasicBSONObject request = new BasicBSONObject();
		request.append(RPCConstants.CS_CMDM_REMOTE_HOST_IP, ip_port);
		request.append(RPCConstants.CS_CMDM_ACTION_DOMAIN, domain);
		request.append(RPCConstants.CS_CMDM_ACTION_NAME, action);
		if (requestData != null) {
			request.append(RPCConstants.CS_CMDM_ACTION_MESSAGE, requestData);
		}

		getClientInstance().sendMessage(request);
	}

	protected BasicBSONObject getResultForID(int resultID) {
		synchronized (resultHashTable) {
			return resultHashTable.get(new Integer(resultID));
		}
	}

	protected BasicBSONObject getRequestResult(BasicBSONObject resultPack) {
		if (resultPack.containsKey((Object) RPCConstants.CS_CMDM_ACTION_MESSAGE)) {
			return (BasicBSONObject) resultPack.get(RPCConstants.CS_CMDM_ACTION_MESSAGE);
		}
		return null;
	}

	protected int getErrorCode(BasicBSONObject resultPack) {
		if (resultPack.containsKey((Object) RPCConstants.CS_CMDM_ACTION_SUBMISSION_ERROR_CODE)) {
			return resultPack.getInt(RPCConstants.CS_CMDM_ACTION_SUBMISSION_ERROR_CODE);
		}
		return Integer.MIN_VALUE;
	}

	protected String getErrorDomain(BasicBSONObject resultPack) {
		if (resultPack.containsKey((Object) RPCConstants.CS_CMDM_ACTION_SUBMISSION_ERROR_DOMAIN)) {
			return resultPack.getString(RPCConstants.CS_CMDM_ACTION_SUBMISSION_ERROR_DOMAIN);
		}
		return "";
	}

	protected String getErrorMessage(BasicBSONObject resultPack) {
		if (resultPack.containsKey((Object) RPCConstants.CS_CMDM_ACTION_SUBMISSION_ERROR_MESSAGE)) {
			return resultPack.getString(RPCConstants.CS_CMDM_ACTION_SUBMISSION_ERROR_MESSAGE);
		}
		return "";
	}

	public Object getInputData() {
		return inputData;
	}

	public void setInputData(Object inputData) {
		this.inputData = inputData;
	}

	abstract protected void executeJob() throws Throwable;

	public RPCClient getClientInstance() {
		return clientInstance;
	}

	@Override
	public void intiHanlder() throws RefException {
		addDomainAction(jobDomain, "answer");

	}

	@Override
	public BasicBSONObject handleAction(String domain, String action, BasicBSONObject actionData) throws RefException {
		if (domain.equals(jobDomain)) {
			if (action.equals("asnwer")) {
				consumeAnswer(actionData);
			}
		}
		return null;
	}
}