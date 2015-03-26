/**
 * 
 */
package it.infn.chaos.mds.rpc.server;

import java.util.concurrent.Executors;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

import org.bson.BasicBSONObject;

/**
 * @author bisegni
 */
public class ActionHandlerExecutor/* implements Runnable*/ {
	private int												threadNumber		= 0;
	private Boolean											done				= false;
	private ScheduledExecutorService						executionService	= null;
	private LinkedBlockingQueue<ActionHandlerExecutionUnit>	hanlderQueue		= new LinkedBlockingQueue<ActionHandlerExecutionUnit>();

	/**
	 * @param threadNumber
	 */
	public ActionHandlerExecutor(int threadNumber) {
		executionService = Executors.newScheduledThreadPool(this.threadNumber = threadNumber);
		//for (int i = 0; i < threadNumber; i++) {
			//executionService.execute(this);
		//}
	}

	/**
	 * @param h
	 * @param d
	 * @throws InterruptedException
	 */
	public void submitAction(RPCActionHadler h, BasicBSONObject d) throws InterruptedException {
		executionService.execute(new ActionHandlerExecutionUnit(h, d));
		//executionService.awaitTermination(500, TimeUnit.MILLISECONDS);
		//ActionHandlerExecutionUnit a= new ActionHandlerExecutionUnit(h, d);
		//a.run();
	}

	/**
	 * @author bisegni
	
	class ActionHandlerExecutionUnit {
		public RPCActionHadler	handler	= null;
		public BasicBSONObject	data	= null;

		public ActionHandlerExecutionUnit(RPCActionHadler h, BasicBSONObject d) {
			handler = h;
			data = d;
		}
	} */

	public void stop() throws InterruptedException {
		synchronized (done) {
			done = true;
		}

		hanlderQueue.isEmpty();
		//for (int i = 0; i < getThreadNumber(); i++) {
			//hanlderQueue.add(new ActionHandlerExecutionUnit(null, null));
		//}
		executionService.shutdown();
		executionService.awaitTermination(600, TimeUnit.SECONDS);
	}

	/*
	 * (non-Javadoc)
	 * @see java.lang.Runnable#run()
	
	public void run() {
		while (!done) {
			BasicBSONObject resultMessage = new BasicBSONObject();
			BasicBSONObject resultPack = new BasicBSONObject();
			ActionHandlerExecutionUnit actionHandler = null;
			try {
				System.out.println("pre hanlderQueue.take() " + Thread.currentThread());
				actionHandler = hanlderQueue.take();
				System.out.println("post hanlderQueue.take() " + Thread.currentThread());
			} catch (InterruptedException e1) {
				e1.printStackTrace();
			}

			if (actionHandler == null || actionHandler.handler == null)
				continue;

			String domain = actionHandler.data.getString(RPCConstants.CS_CMDM_ACTION_DOMAIN);
			String action = actionHandler.data.getString(RPCConstants.CS_CMDM_ACTION_NAME);
			if (domain == null && action == null)
				continue;
			BasicBSONObject actionMessage = (BasicBSONObject) (actionHandler.data.containsField(RPCConstants.CS_CMDM_ACTION_MESSAGE) ? actionHandler.data.get(RPCConstants.CS_CMDM_ACTION_MESSAGE) : null);
			// get info for replay
			try {

				actionMessage = actionHandler.handler._handleAction(domain, action, actionMessage);
				resultPack.append(RPCConstants.CS_CMDM_ACTION_SUBMISSION_ERROR_CODE, 0);
				if (actionMessage != null)
					resultPack.append(RPCConstants.CS_CMDM_ACTION_MESSAGE, actionMessage);
			} catch (RefException e) {
				RPCUtils.addRefExceptionToBson(resultPack, e);
			}

			String responseAddress = actionHandler.data.containsField(RPCConstants.CS_CMDM_REMOTE_HOST_ANSWER_IP) ? actionHandler.data.getString(RPCConstants.CS_CMDM_REMOTE_HOST_ANSWER_IP) : null;
			String reponseDomain = actionHandler.data.containsField(RPCConstants.CS_CMDM_ANSWER_DOMAIN) ? actionHandler.data.getString(RPCConstants.CS_CMDM_ANSWER_DOMAIN) : null;
			String reponseAction = actionHandler.data.containsField(RPCConstants.CS_CMDM_ANSWER_ACTION) ? actionHandler.data.getString(RPCConstants.CS_CMDM_ANSWER_ACTION) : null;
			Integer responseID = actionHandler.data.containsField(RPCConstants.CS_CMDM_REMOTE_HOST_ANSWER_MESSAGE_ID) ? actionHandler.data.getInt(RPCConstants.CS_CMDM_REMOTE_HOST_ANSWER_MESSAGE_ID) : null;

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
				System.out.println(resultMessage.toString());
				SingletonServices.getInstance().getMdsRpcClient().sendMessage(resultMessage);
			} catch (Throwable e) {
			}
		}
		System.out.println("Done executing ActionHandlerExecutionUnit");
	}
 */
	public int getThreadNumber() {
		return threadNumber;
	}
}
