package it.infn.chaos.chaos.common.rpc;

import it.infn.chaos.chaos.common.exception.ChaosException;

import java.io.IOException;
import java.util.Hashtable;

/**
 * @author bisegni
 */
abstract public class RPCServer {
	private final String						actionHanlderFormat	= "%s_%s";
	private Hashtable<String, RPCActionHadler>	actionHanlder		= new Hashtable<String, RPCActionHadler>();

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
	 * 
	 */
	public void addDomainActionHanlder(String domain, String action, RPCActionHadler handler) throws ChaosException {
		if (domain == null || action == null || handler == null)
			throw new ChaosException("Ivalid parameter", 0, "RPCServer::addDomainActionHanlder");
		String key = String.format(actionHanlderFormat, domain, action);
		actionHanlder.put(key, handler);
	}
	
	/**
	 * 
	 * @param domain
	 * @param action
	 * @return
	 * @throws RefException 
	 */
	public RPCActionHadler getHandlerForDomainAndAction(String domain, String action) throws ChaosException {
		if (domain == null || action == null)
			throw new ChaosException("Ivalid parameter", 0, "RPCServer::addDomainActionHanlder");
		String key = String.format(actionHanlderFormat, domain, action);
		return actionHanlder.get(key);
	}
}
