/**
 * 
 */
package it.infn.chaos.mds.rpc.server;

import it.infn.chaos.mds.RPCConstants;
import it.infn.chaos.mds.SingletonServices;

import java.util.Vector;

import org.bson.BasicBSONDecoder;
import org.bson.BasicBSONEncoder;
import org.bson.BasicBSONObject;
import org.ref.common.exception.RefException;
import org.zeromq.ZContext;
import org.zeromq.ZMQ.Socket;

import zmq.ZMQ;

/**
 * @author bisegni
 * 
 */
public class JMQRPCServer extends RPCServer implements Runnable {
	private ZContext context = null;
	private BasicBSONDecoder bDecoder = new BasicBSONDecoder();
	private Vector<Thread> threadVector = new Vector<Thread>();
	private boolean work = true;
	private int receivePort = 0;
	private ActionHandlerExecutor handlerExecution = null;

	
	private void doaction(RPCActionHadler handler,BasicBSONObject data) throws Throwable{
		
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
			
				//System.out.println(resultMessage.toString());
				SingletonServices.getInstance().getMdsRpcClient().sendMessage(resultMessage);
			
			//System.out.println("Done executing ActionHandlerExecutionUnit");
		}
	
	/**
	 * 
	 * @param threadNumber
	 */
	public JMQRPCServer(int threadNumber) {
		handlerExecution = new ActionHandlerExecutor(threadNumber);
	}

	
	/*
	 * (non-Javadoc)
	 * 
	 * @see it.infn.chaos.mds.rpc.server.RPCServer#init(int)
	 */
	@Override
	public void init(int receivePort) throws Throwable {
		System.out.println("JMQRPCServer initialization");
		this.receivePort = receivePort;
		context = new ZContext(2);
		if (context == null)
			throw new RefException("Context Creation", 0, "JMQRPCServer::init");
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see it.infn.chaos.mds.rpc.server.RPCServer#start()
	 */
	@Override
	public void start() throws Throwable {
		work = true;
		for (int idx = 0; idx < handlerExecution.getThreadNumber(); idx++) {
			Thread localT = new Thread(this);
			localT.start();
			threadVector.add(localT);
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see it.infn.chaos.mds.rpc.server.RPCServer#deinit()
	 */
	@Override
	public void deinit() throws Throwable {
		context.destroy();
		work = false;
		for (int idx = 0; idx < handlerExecution.getThreadNumber(); idx++) {
			threadVector.elementAt(idx).join();
		}
		handlerExecution.stop();
	}

	Socket createSocket(){
		Socket rep_socket = context.createSocket(ZMQ.ZMQ_REP);
		if (rep_socket == null) {
			System.out.println("Error creating socket");
			return null;
		}

		if (rep_socket.bind("tcp://*:" + receivePort) == 0) {
			System.out.println("JMQRPCServer initilized");
			return null;
		}
		//rep_socket.setSendTimeOut(5000);
		// socketSlot.socket.setSendTimeOut(10000);
		//rep_socket.setReceiveTimeOut(5000);
		rep_socket.setReceiveTimeOut(-1);
		rep_socket.setSndHWM(1);
		rep_socket.setRcvHWM(1);
		rep_socket.setLinger(0);
		//rep_socket.setReceiveBufferSize(1024);
		rep_socket.setTCPKeepAlive(0);
		return rep_socket;
		
	}
	@Override
	public void run() {
		byte[] buffer = null;
		Socket rep_socket = createSocket();
		while (work) {
			buffer = null;
			buffer = rep_socket.recv();
			if(buffer==null)
				continue;
			
			BasicBSONObject bsonData = (BasicBSONObject) bDecoder.readObject(buffer);
			String domain = bsonData.getString(RPCConstants.CS_CMDM_ACTION_DOMAIN);
			String action = bsonData.getString(RPCConstants.CS_CMDM_ACTION_NAME);
			System.out.println("recv "+domain + " action:"+action +" reply to:"+bsonData.getString(RPCConstants.CS_CMDM_REMOTE_HOST_ANSWER_IP));

			RPCActionHadler actionHandler;
			try {
				if(domain == null ||
						action == null) {
					throw new RefException("No domain or action have been found");
				}
				actionHandler = getHandlerForDomainAndAction(domain, action);
				if (actionHandler == null) {
					rep_socket.send(getResponseRawType("No action handler", "MSGPackRPCServer", 1));
					return;
				}
				try{
					if(action.contains("register")){
						//make registering synchronous
						doaction(actionHandler, bsonData);	
					} else {
						//doaction(actionHandler, bsonData);	
						handlerExecution.submitAction(actionHandler, bsonData);
					}
				//	
				} catch(Throwable e){
					System.out.println("ECCEZIONE:"+e.getMessage());
	//				rep_socket.recv(buffer, 0, 1024*1024, 0);
					//rep_socket.disconnect("tcp://"+bsonData.getString(RPCConstants.CS_CMDM_REMOTE_HOST_ANSWER_IP));	
					//rep_socket.close();
					
				}
				rep_socket.send(getResponseRawType(null, null, 0));
			} catch (RefException e) {
				rep_socket.send(getResponseRawType(e.getExceptionDomain(), e.getMessage(), e.getExceptionCode()));
			} 
		}
	}

	/**
	 * @param submissionResult
	 * @param errorDomain
	 * @param errorMsg
	 * @param errCode
	 * @return
	 */
	private byte[] getResponseRawType(String errorDomain, String errorMsg, int errCode) {
		BasicBSONObject bbObj = new BasicBSONObject();
		BasicBSONEncoder enc = new BasicBSONEncoder();

		// bbObj.append(RPCConstants.CS_CMDM_ACTION_SUBMISSION_ERROR_CODE, submissionResult);
		// fill bson with error
		RPCUtils.addRefExceptionElementToBson(bbObj, errorDomain, errorMsg, errCode);
		return enc.encode(bbObj);
	}
}
