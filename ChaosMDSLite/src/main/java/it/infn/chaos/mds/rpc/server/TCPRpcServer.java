/**
 * 
 */
package it.infn.chaos.mds.rpc.server;

import it.infn.chaos.mds.RPCConstants;

import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

import org.bson.BasicBSONDecoder;
import org.bson.BasicBSONEncoder;
import org.bson.BasicBSONObject;
import org.ref.common.exception.RefException;
import org.ref.common.helper.ExceptionHelper;
import org.ref.common.helper.StringHelper;

/**
 * @author bisegni
 * 
 */
public class TCPRpcServer extends RPCServer implements Runnable {
	private BasicBSONDecoder		bDecoder			= new BasicBSONDecoder();
	private boolean					work				= true;
	private ActionHandlerExecutor	handlerExecution	= null;
	private ServerSocket			incomingSocket		= null;
	private Thread					localT				= null;

	/*
	 * (non-Javadoc)
	 * 
	 * @see it.infn.chaos.mds.rpc.server.RPCServer#init(int)
	 */
	@Override
	public void init(int port) throws Throwable {
		System.out.println("TCPRpcServer initialization");
		incomingSocket = new ServerSocket(port);
		handlerExecution = new ActionHandlerExecutor(2);

	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see it.infn.chaos.mds.rpc.server.RPCServer#start()
	 */
	@Override
	public void start() throws Throwable {
		localT = new Thread(this);
		localT.start();
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see it.infn.chaos.mds.rpc.server.RPCServer#deinit()
	 */
	@Override
	public void deinit() throws Throwable {
		work = false;
		incomingSocket.close();
		localT.join();
		handlerExecution.stop();

	}

	@Override
	public void run() {
		while (work) {
			byte[] buffer = new byte[1024];
			Socket connectionSocket = null;
			DataInputStream inFromClient = null;
			DataOutputStream outToClient = null;
			try {
				connectionSocket = incomingSocket.accept();

				inFromClient = new DataInputStream(connectionSocket.getInputStream());

				outToClient = new DataOutputStream(connectionSocket.getOutputStream());

				int readed = 0;
				ByteArrayOutputStream baos = new ByteArrayOutputStream();
				do {
					// i have some data
					if ((readed = inFromClient.read(buffer)) != -1)
						baos.write(buffer, 0, readed);
				} while (readed >= 1024);
				// i have all the byte
				
				BasicBSONObject bsonData = (BasicBSONObject) bDecoder.readObject(baos.toByteArray());
				System.out.println(bsonData);
				String domain = bsonData.getString(RPCConstants.CS_CMDM_ACTION_DOMAIN);
				String action = bsonData.getString(RPCConstants.CS_CMDM_ACTION_NAME);

				RPCActionHadler actionHandler;
				try {
					actionHandler = getHandlerForDomainAndAction(domain, action);
					if (actionHandler == null) {
						outToClient.write(getResponseRawType("No action handler", "MSGPackRPCServer", 1));
						return;
					}
					handlerExecution.submitAction(actionHandler, bsonData);
					outToClient.write(getResponseRawType(null, null, 0));
				} catch (RefException e) {
					outToClient.write(getResponseRawType(e.getExceptionDomain(), e.getMessage(), e.getExceptionCode()));
				} catch (InterruptedException e) {
					outToClient.write(getResponseRawType(null, e.getMessage(), 0));
				}
			} catch (IOException e1) {
				try {
					outToClient.write(getResponseRawType(null, ExceptionHelper.getInstance().putExcetpionStackToString(e1), 0));
				} catch (IOException e) {
					e.printStackTrace();
				}
			} catch (Throwable e) {
				try {
					outToClient.write(getResponseRawType(null, ExceptionHelper.getInstance().putExcetpionStackToString(e), 0));
				} catch (IOException e1) {
					e1.printStackTrace();
				}
			} finally {
				try {
					inFromClient.close();
					outToClient.close();
					connectionSocket.close();
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		}
		System.out.println("End of TCPRpcServer");
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
