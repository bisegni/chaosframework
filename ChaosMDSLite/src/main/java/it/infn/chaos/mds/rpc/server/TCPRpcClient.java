package it.infn.chaos.mds.rpc.server;

import it.infn.chaos.mds.RPCConstants;

import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.net.InetSocketAddress;
import java.net.Socket;

import org.bson.BasicBSONObject;

public class TCPRpcClient extends RPCClient {

	@Override
	public void init(int port) throws Throwable {
		// TODO Auto-generated method stub

	}

	@Override
	public void start() throws Throwable {
		// TODO Auto-generated method stub

	}

	@Override
	public void deinit() throws Throwable {
		// TODO Auto-generated method stub

	}

	@Override
	public void sendMessage(BasicBSONObject messageData) throws Throwable {
		if (!messageData.containsField(RPCConstants.CS_CMDM_REMOTE_HOST_IP))
			return;
		byte[] buffer = new byte[1024];
		String[] addressInfo = messageData.getString(RPCConstants.CS_CMDM_REMOTE_HOST_IP).split(":");

		Socket clientSocket = new Socket();
		clientSocket.connect(new InetSocketAddress(addressInfo[0], Integer.parseInt(addressInfo[1])), 5000);
		
		DataOutputStream outToServer = new DataOutputStream(clientSocket.getOutputStream());
		DataInputStream inFromServer = new DataInputStream(clientSocket.getInputStream());
		try {
			byte[] rawData = encoder.encode(messageData);
			outToServer.write(rawData);

			int readed = 0;
			ByteArrayOutputStream baos = new ByteArrayOutputStream();
			do {
				// i have some data
				if((readed = inFromServer.read(buffer)) != -1)
					baos.write(buffer, 0, readed);
			} while (readed >= 1024);
			
			BasicBSONObject bsonResult = (BasicBSONObject) decoder.readObject(baos.toByteArray());
			System.out.println("Submission result->" + bsonResult);
		} catch (Throwable e) {
			throw e;
		} finally {
			if (clientSocket != null)
				clientSocket.close();
		}

	}
}
