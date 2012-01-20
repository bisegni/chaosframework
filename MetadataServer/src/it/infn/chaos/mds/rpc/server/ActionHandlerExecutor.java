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

	@Override
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

			// get info for replay
			try {
				resultObject = actionHandler.handler._handleAction(domain, action, actionHandler.data);

				if (resultObject != null) {
					String address = actionHandler.data.containsField(RPCConstants.CS_CMDM_REMOTE_HOST_RESPONSE_IP) ? actionHandler.data.getString(RPCConstants.CS_CMDM_REMOTE_HOST_RESPONSE_IP) : null;
					if (address == null) {
						// i need to check if someone ha set another ip as destination
						if (!resultObject.containsField(RPCConstants.CS_CMDM_REMOTE_HOST_IP))
							continue;
					} else {
						resultObject.remove(RPCConstants.CS_CMDM_REMOTE_HOST_IP);
						resultObject.append(RPCConstants.CS_CMDM_REMOTE_HOST_IP, address);
					}

					String answerKey = actionHandler.data.containsField(RPCConstants.CS_CMDM_REMOTE_HOST_RESPONSE_ID) ? actionHandler.data.getString(RPCConstants.CS_CMDM_REMOTE_HOST_RESPONSE_ID) : null;
					if (answerKey != null) {
						resultObject.append(RPCConstants.CS_CMDM_REMOTE_HOST_RESPONSE_ID, answerKey);
					}
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
