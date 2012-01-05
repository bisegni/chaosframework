/**
 * 
 */
package it.infn.chaos.mds.rpc.server;

import java.net.UnknownHostException;
import java.util.Hashtable;
import java.util.StringTokenizer;

import org.bson.BasicBSONDecoder;
import org.bson.BasicBSONObject;
import org.msgpack.MessagePackObject;
import org.msgpack.object.RawType;
import org.msgpack.rpc.Client;
import org.msgpack.rpc.loop.EventLoop;
import org.ref.common.exception.RefException;

/**
 * @author bisegni
 */
public class MSGPackRPCClient extends RPCClient {
	private EventLoop					loop		= null;
	private Hashtable<String, Address>	clienHash	= new Hashtable<String, Address>();

	/**
	 * @throws Throwable
	 */
	public void sendMessage(String addressAndPort, BasicBSONObject messageData) throws Throwable {
		synchronized (encoder) {
			MessagePackObject result = null;
			Client cli = getClientForAddress(addressAndPort);
			try {
				byte[] rawData = encoder.encode(messageData);
				Object[] data = new Object[1];
				data[0] = RawType.create(rawData);
				// cli.notifyApply("chaos_rpc", data);

				result = cli.sendRequest("chaos_rpc", data).get();
				BasicBSONObject bsonResult = (BasicBSONObject) decoder.readObject(result.asByteArray());
				System.out.println("Submission result->" + bsonResult);
			} catch (Throwable e) {
				throw e;
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
	synchronized protected Client getClientForAddress(String addressAndPort) throws UnknownHostException, RefException {
		Address address = null;
		if (addressAndPort == null)
			throw new RefException("Address can't be null");

		if ((address = clienHash.get(addressAndPort)) == null) {
			int idx = 0;
			address = new Address();
			StringTokenizer tok = new StringTokenizer(addressAndPort, ":");
			if (tok.countTokens() != 2)
				throw new RefException("Bad address");

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
