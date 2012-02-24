/**
 * 
 */
package it.infn.chaos.mds.rpc.server;

import it.infn.chaos.mds.RPCConstants;
import it.infn.chaos.mds.SingletonServices;

import java.util.concurrent.*;

import org.bson.BasicBSONObject;
import org.ref.common.exception.RefException;

/**
 * @author bisegni
 */
public class ActionHandlerExecutor implements Runnable {
	private int												threadNumber		= 0;
	private Boolean											done				= false;
	private ScheduledExecutorService						executionService	= null;
	private LinkedBlockingQueue<ActionHandlerExecutionUnit>	hanlderQueue		= new LinkedBlockingQueue<ActionHandlerExecutionUnit>();

	/**
	 * @param threadNumber
	 */
	public ActionHandlerExecutor(int threadNumber) {
		this.threadNumber = threadNumber;
		executionService = Executors.newScheduledThreadPool(threadNumber);
		for (int i = 0; i < threadNumber; i++) {
			executionService.execute(this);
		}
	}

	/**
	 * @param h
	 * @param d
	 * @throws InterruptedException
	 */
	public void submitAction(RPCActionHadler h, BasicBSONObject d) throws InterruptedException {
		synchronized (done) {
			hanlderQueue.put(new ActionHandlerExecutionUnit(h, d));
		}
	}

	/**
	 * @author bisegni
	 */
	class ActionHandlerExecutionUnit {
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
	}

	public void stop() throws InterruptedException {
		synchronized (done) {
			done = true;
		}

		hanlderQueue.isEmpty();
		for (int i = 0; i < threadNumber; i++) {
			hanlderQueue.add(new ActionHandlerExecutionUnit(null, null));
		}
		executionService.shutdown();
		executionService.awaitTermination(10, TimeUnit.SECONDS);
	}

	/*
	 * (non-Javadoc)
	 * @see java.lang.Runnable#run()
	 */
	public void run() {
		while (!done) {
			BasicBSONObject resultObject = null;
			ActionHandlerExecutionUnit actionHandler = null;
			try {
				actionHandler = hanlderQueue.take();
			} catch (InterruptedException e1) {
				e1.printStackTrace();
			}

			if (actionHandler == null
				|| actionHandler.handler == null)
				continue;

			String domain = actionHandler.data.getString(RPCConstants.CS_CMDM_ACTION_DOMAIN);
			String action = actionHandler.data.getString(RPCConstants.CS_CMDM_ACTION_NAME);
			if (domain == null
				&& action == null)
				continue;
			BasicBSONObject actionMessage = (BasicBSONObject) (actionHandler.data.containsField(RPCConstants.CS_CMDM_ACTION_MESSAGE)?actionHandler.data.get(RPCConstants.CS_CMDM_ACTION_MESSAGE):null);
			// get info for replay
			try {
				
				actionMessage = actionHandler.handler._handleAction(domain, action, actionMessage);

				if (actionMessage != null) {
					resultObject = new BasicBSONObject();
					String responseAddress = actionHandler.data.containsField(RPCConstants.CS_CMDM_REMOTE_HOST_ANSWER_IP) ? actionHandler.data.getString(RPCConstants.CS_CMDM_REMOTE_HOST_ANSWER_IP) : null;
					String reponseDomain = actionHandler.data.containsField(RPCConstants.CS_CMDM_ANSWER_DOMAIN) ? actionHandler.data.getString(RPCConstants.CS_CMDM_ANSWER_DOMAIN) : null;
					String reponseAction = actionHandler.data.containsField(RPCConstants.CS_CMDM_ANSWER_ACTION) ? actionHandler.data.getString(RPCConstants.CS_CMDM_ANSWER_ACTION) : null;
					Integer responseID = actionHandler.data.containsField(RPCConstants.CS_CMDM_REMOTE_HOST_ANSWER_MESSAGE_ID) ? actionHandler.data.getInt(RPCConstants.CS_CMDM_REMOTE_HOST_ANSWER_MESSAGE_ID) : null;

					if (responseAddress != null) {
						resultObject.append(RPCConstants.CS_CMDM_REMOTE_HOST_IP, responseAddress);
					}

					if (reponseDomain != null) {
						resultObject.append(RPCConstants.CS_CMDM_ACTION_DOMAIN, reponseDomain);
					}

					if (reponseAction != null) {
						resultObject.append(RPCConstants.CS_CMDM_ACTION_NAME, reponseAction);
					}

					if (responseID != null) {
						actionMessage.append(RPCConstants.CS_CMDM_MESSAGE_ID, responseID);
					}

					//add the action message
					resultObject.put(RPCConstants.CS_CMDM_ACTION_MESSAGE, actionMessage);
					SingletonServices.getInstance().getMdsRpcClient().sendMessage(resultObject);
				}
			} catch (RefException e) {
				e.printStackTrace();
			} catch (Throwable e) {
				e.printStackTrace();
			} finally {
				resultObject = null;
			}

		}
	}
}
