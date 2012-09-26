/**
 * 
 */
package it.infn.chaos.chaos.common.rpc.msgpack;

import it.infn.chaos.chaos.common.exception.ChaosException;
import it.infn.chaos.chaos.common.rpc.RPCClient;
import it.infn.chaos.chaos.common.rpc.RPCConstants;

import java.net.UnknownHostException;
import java.util.Hashtable;
import java.util.StringTokenizer;

import org.bson.BasicBSONObject;
import org.msgpack.rpc.Client;
import org.msgpack.rpc.loop.EventLoop;
import org.msgpack.type.Value;
import org.msgpack.type.ValueFactory;

/**
 * @author bisegni
 */
public class MSGPackRPCClient extends RPCClient {
	private EventLoop					loop		= null;
	private Hashtable<String, Address>	clienHash	= new Hashtable<String, Address>();

	/**
	 * @throws Throwable
	 */
	public void sendMessage(BasicBSONObject messageData) throws ChaosException {
		synchronized (encoder) {
			Value result = null;
			Client cli = null;
			String serverAddress = messageData.containsField(RPCConstants.CS_CMDM_REMOTE_HOST_IP) ? messageData.getString(RPCConstants.CS_CMDM_REMOTE_HOST_IP) : null;
			try {
				cli = getClientForAddress(serverAddress);
				byte[] rawData = encoder.encode(messageData);
				Object[] data = new Object[1];
				data[0] = ValueFactory.createRawValue(rawData);
				result = cli.sendRequest("chaos_rpc", data).get();
				BasicBSONObject bsonResult = (BasicBSONObject) decoder.readObject(result.asRawValue().getByteArray());
				System.out.println("Submission result->" + bsonResult);
			} catch (ChaosException e) {
				throw e;
			} catch (UnknownHostException e) {
				throw new ChaosException("Unknown Host", 0, "MSGPackRPCClient::sendMessage");
			} catch (InterruptedException e) {
				throw new ChaosException("Interrupted Exception", 1, "MSGPackRPCClient::sendMessage");
			} finally {
				cli.close();
			}

		}
	}

	/**
	 * @param addressAndPort
	 * @return
	 * @throws UnknownHostException
	 * @throws RefException
	 */
	synchronized protected Client getClientForAddress(String addressAndPort) throws UnknownHostException, ChaosException {
		Address address = null;
		if (addressAndPort == null)
			throw new ChaosException("Address can't be null", 0, "MSGPackRPCClient::getClientForAddress");

		if ((address = clienHash.get(addressAndPort)) == null) {
			int idx = 0;
			address = new Address();
			StringTokenizer tok = new StringTokenizer(addressAndPort, ":");
			if (tok.countTokens() != 2)
				throw new ChaosException("Bad address", 0, "MSGPackRPCClient::getClientForAddress");

			while (tok.hasMoreElements()) {
				String addTok = (String) tok.nextToken();

				switch (++idx) {
					case 1:
						address.host = addTok;
					break;
					case 2:
						address.port = Integer.parseInt(addTok);
					break;
				}
			}
			clienHash.put(addressAndPort, address);
		}

		return new Client(address.host, address.port, loop);
	}

	@Override
	public void init(int port) throws Throwable {
		loop = EventLoop.start();

	}

	@Override
	public void start() throws Throwable {

	}

	@Override
	public void deinit() throws Throwable {
		loop.shutdown();
	}

	class Address {
		String	host;
		int		port;
	}
}
