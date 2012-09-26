/**
 * 
 */
package it.infn.chaos.chaos.common.rpc;

import java.io.IOException;
import java.net.UnknownHostException;

import org.bson.BasicBSONDecoder;
import org.bson.BasicBSONEncoder;
import org.bson.BasicBSONObject;

/**
 * @author bisegni
 */
abstract public class RPCClient {
	protected BasicBSONEncoder	encoder	= new BasicBSONEncoder();
	protected BasicBSONDecoder	decoder	= new BasicBSONDecoder();

	/**
	 * @throws IOException
	 */
	abstract public void init(int port) throws Throwable;

	/**
	 * @throws IOException
	 */
	abstract public void start() throws Throwable;

	/**
	 * @throws IOException
	 */
	abstract public void deinit() throws Throwable;

	/**
	 * @throws RefException
	 * @throws UnknownHostException
	 */
	abstract public void sendMessage(BasicBSONObject messageData) throws Throwable;

}
