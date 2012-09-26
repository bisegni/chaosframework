/**
 * 
 */
package it.infn.chaos.chaos.common;

import it.infn.chaos.chaos.common.exception.ChaosException;
import it.infn.chaos.chaos.common.rpc.RPCActionHadler;
import it.infn.chaos.chaos.common.rpc.RPCClient;
import it.infn.chaos.chaos.common.rpc.RPCServer;


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
	public void addHandler(Class<? extends RPCActionHadler> handlerClass) throws InstantiationException, IllegalAccessException, ChaosException {
		RPCActionHadler hanlder = handlerClass.newInstance();
		hanlder.intiHanlder();
		hanlder.registerDomanAndNameForHandler(getMdsRpcServer());
	}
}
