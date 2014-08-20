package it.infn.chaos.mds.rpc.server;

import it.infn.chaos.mds.RPCConstants;

import org.bson.BasicBSONObject;
import org.zeromq.ZContext;
import org.zeromq.ZMQ.Socket;

import zmq.ZMQ;

public class JMQRPCClient extends RPCClient {
	private ZContext context = null;
	private int threadNUmber = 0;

	public JMQRPCClient(int threadNUmber) {
		this.threadNUmber = threadNUmber;
	}

	@Override
	public void init(int port) throws Throwable {
		context = new ZContext(this.threadNUmber);
	}

	@Override
	public void start() throws Throwable {

	}

	@Override
	public void deinit() throws Throwable {
		context.destroy();

	}

	@Override
	public void sendMessage(BasicBSONObject messageData) throws Throwable {
		Socket reqSocket = context.createSocket(ZMQ.ZMQ_REQ);
		String serverAddress = "tcp://" + (messageData.containsField(RPCConstants.CS_CMDM_REMOTE_HOST_IP) ? messageData.getString(RPCConstants.CS_CMDM_REMOTE_HOST_IP) : null);
		reqSocket.connect(serverAddress);
		try {
			byte[] rawData = encoder.encode(messageData);

			if (!reqSocket.send(rawData)) {
				System.err.println("JMQRPCClient - Error sending data");
				return;
			}

			if ((rawData = reqSocket.recv()) == null) {
				System.err.println("JMQRPCClient - Error receiving the answer data");
				return;
			}
			BasicBSONObject bsonResult = (BasicBSONObject) decoder.readObject(rawData);
			System.out.println("Submission result->" + bsonResult);
		} catch (Throwable e) {
			throw e;
		} finally {
			if (reqSocket != null)
				reqSocket.close();
		}

	}

}
