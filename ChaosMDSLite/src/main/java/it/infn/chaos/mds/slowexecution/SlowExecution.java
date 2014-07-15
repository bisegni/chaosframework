package it.infn.chaos.mds.slowexecution;

import it.infn.chaos.mds.RPCConstants;
import it.infn.chaos.mds.SingletonServices;
import it.infn.chaos.mds.rpc.server.RPCActionHadler;
import it.infn.chaos.mds.rpc.server.RPCClient;

import java.util.Hashtable;
import java.util.UUID;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.Semaphore;
import java.util.concurrent.atomic.AtomicInteger;

import org.bson.BasicBSONObject;
import org.ref.common.exception.RefException;

public class SlowExecution {
	private AtomicInteger								counterRequestID	= new AtomicInteger();
	private ScheduledExecutorService					executionService	= null;
	private Hashtable<String, Class<SlowExecutioJob>>	mapAliasJob			= new Hashtable<String, Class<SlowExecutioJob>>();

	public SlowExecution(int threadNumber) {
		executionService = Executors.newScheduledThreadPool(threadNumber);
	}

	public void registerSlowControlJob(String scJobAlias, Class<SlowExecutioJob> job) {
		mapAliasJob.put(scJobAlias, job);
	}

	public boolean submitJob(String alias, BasicBSONObject jobData) throws InstantiationException, IllegalAccessException {
		if (!mapAliasJob.contains(alias))
			return false;
		SlowExecutioJob job = mapAliasJob.get(alias).newInstance();
		job.setInputData(jobData);
		executionService.execute(job);
		return true;
	}

	abstract public class SlowExecutioJob extends RPCActionHadler implements Runnable {
		private BasicBSONObject						inputData		= null;
		private RPCClient							clientInstance	= null;
		private String								jobDomain		= UUID.randomUUID().toString().substring(0, 10);
		private final Semaphore						available		= new Semaphore(1);
		private Hashtable<Integer, BasicBSONObject>	resultHashTable	= new Hashtable<Integer, BasicBSONObject>();
		

		public SlowExecutioJob() throws RefException {
			clientInstance = SingletonServices.getInstance().getMdsRpcClient();

			registerDomanAndNameForHandler(SingletonServices.getInstance().getMdsRpcServer());

		}

		@Override
		public void run() {
			// TODO Auto-generated method stub
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
		}

		private void consumeAnswer(BasicBSONObject actionData) {
			if(actionData.containsKey((Object)RPCConstants.CS_CMDM_MESSAGE_ID)) {
				synchronized (resultHashTable) {
					int resultID = actionData.getInt(RPCConstants.CS_CMDM_MESSAGE_ID);
					resultHashTable.put(new Integer(resultID), actionData);
				}
			}
			
		}

		protected BasicBSONObject sendRequest(String domain, String action, BasicBSONObject requestData, Integer requestID, boolean wait) throws Throwable {
			BasicBSONObject request = new BasicBSONObject();
			request.append(RPCConstants.CS_CMDM_REMOTE_HOST_ANSWER_IP, "");
			request.append(RPCConstants.CS_CMDM_ACTION_DOMAIN, domain);
			request.append(RPCConstants.CS_CMDM_ACTION_NAME, action);
			if (requestData != null) {
				// request.append(RPCConstants.CS_CMDM_ACTION_MESSAGE, requestData);
			}

			// add current server as
			int resultID = counterRequestID.incrementAndGet();
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

		protected BasicBSONObject getResultForID(int resultID) {
			synchronized (resultHashTable) {
				return resultHashTable.get(new Integer(resultID));
			}
		}
		
		protected BasicBSONObject getRequestResult(BasicBSONObject resultPack) {
			if(resultPack.containsKey((Object)RPCConstants.CS_CMDM_ACTION_MESSAGE)) {
				return (BasicBSONObject) resultPack.get(RPCConstants.CS_CMDM_ACTION_MESSAGE);
			}
			return null;
		}
		
		protected int getErrorCode(BasicBSONObject resultPack) {
			if(resultPack.containsKey((Object)RPCConstants.CS_CMDM_ACTION_SUBMISSION_ERROR_CODE)) {
				return resultPack.getInt(RPCConstants.CS_CMDM_ACTION_SUBMISSION_ERROR_CODE);
			}
			return Integer.MIN_VALUE;
		}
		
		protected String getErrorDomain(BasicBSONObject resultPack) {
			if(resultPack.containsKey((Object)RPCConstants.CS_CMDM_ACTION_SUBMISSION_ERROR_DOMAIN)) {
				return resultPack.getString(RPCConstants.CS_CMDM_ACTION_SUBMISSION_ERROR_DOMAIN);
			}
			return "";
		}
		
		protected String getErrorMessage(BasicBSONObject resultPack) {
			if(resultPack.containsKey((Object)RPCConstants.CS_CMDM_ACTION_SUBMISSION_ERROR_MESSAGE)) {
				return resultPack.getString(RPCConstants.CS_CMDM_ACTION_SUBMISSION_ERROR_MESSAGE);
			}
			return "";
		}
		public BasicBSONObject getInputData() {
			return inputData;
		}

		public void setInputData(BasicBSONObject inputData) {
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
}
