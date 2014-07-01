/**
 * 
 */
package it.infn.chaos.mds.rpc.server;

import it.infn.chaos.mds.RPCConstants;

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

	@Override
	public void run() {
		byte[] buffer = null;
		Socket rep_socket = context.createSocket(ZMQ.ZMQ_DEALER);
		if (rep_socket == null) {
			System.out.println("Error creating socket");
			return;
		}

		if (rep_socket.bind("tcp://*:" + receivePort) == 0) {
			System.out.println("JMQRPCServer initilized");
			return;
		}

		while (work) {
			buffer = rep_socket.recv();

			BasicBSONObject bsonData = (BasicBSONObject) bDecoder.readObject(buffer);

			String domain = bsonData.getString(RPCConstants.CS_CMDM_ACTION_DOMAIN);
			String action = bsonData.getString(RPCConstants.CS_CMDM_ACTION_NAME);

			RPCActionHadler actionHandler;
			try {
				actionHandler = getHandlerForDomainAndAction(domain, action);
				if (actionHandler == null) {
					rep_socket.send(getResponseRawType("No action handler", "MSGPackRPCServer", 1));
					return;
				}
				handlerExecution.submitAction(actionHandler, bsonData);
				rep_socket.send(getResponseRawType(null, null, 0));
			} catch (RefException e) {
				rep_socket.send(getResponseRawType(e.getExceptionDomain(), e.getMessage(), e.getExceptionCode()));
			} catch (InterruptedException e) {
				e.printStackTrace();
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
