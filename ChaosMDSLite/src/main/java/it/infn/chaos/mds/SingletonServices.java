/**
 * 
 */
package it.infn.chaos.mds;

import it.infn.chaos.mds.rpc.server.RPCActionHadler;
import it.infn.chaos.mds.rpc.server.RPCClient;
import it.infn.chaos.mds.rpc.server.RPCServer;

import org.ref.common.exception.RefException;

/**
 * @author bisegni
 */
public class SingletonServices {
	private RPCServer					mdsRpcServer	= null;
	private RPCClient					mdsRpcClient	= null;
	static private SingletonServices	instance		= null;

	/**
	 * 
	 */
	private SingletonServices() {

	}

	/**
	 * @return
	 */
	synchronized public static SingletonServices getInstance() {
		if (instance == null) {
			instance = new SingletonServices();
		}
		return instance;
	}

	/**
	 * @return the mdsRpcServer
	 */
	public RPCServer getMdsRpcServer() {
		return mdsRpcServer;
	}

	/**
	 * @param mdsRpcServer
	 *            the mdsRpcServer to set
	 */
	public void setMdsRpcServer(RPCServer mdsRpcServer) {
		this.mdsRpcServer = mdsRpcServer;
	}

	/**
	 * @return the mdsRpcClient
	 */
	public RPCClient getMdsRpcClient() {
		return mdsRpcClient;
	}

	/**
	 * @param mdsRpcClient
	 *            the mdsRpcClient to set
	 */
	public void setMdsRpcClient(RPCClient mdsRpcClient) {
		this.mdsRpcClient = mdsRpcClient;
	}

	/**
	 * @param handlerClass
	 * @throws IllegalAccessException 
	 * @throws InstantiationException 
	 * @throws RefException 
	 */
	public void addHandler(Class<? extends RPCActionHadler> handlerClass) throws InstantiationException, IllegalAccessException, RefException {
		RPCActionHadler hanlder = handlerClass.newInstance();
		hanlder.intiHanlder();
		hanlder.registerDomanAndNameForHandler(getMdsRpcServer());
	}
}
