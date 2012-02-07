package it.infn.chaos.mds.rpc.server;

import java.io.IOException;
import java.util.Hashtable;

import org.ref.common.exception.RefException;

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
	public void addDomainActionHanlder(String domain, String action, RPCActionHadler handler) throws RefException {
		if (domain == null || action == null || handler == null)
			throw new RefException("Ivalid parameter");
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
	public RPCActionHadler getHandlerForDomainAndAction(String domain, String action) throws RefException {
		if (domain == null || action == null)
			throw new RefException("Ivalid parameter");
		String key = String.format(actionHanlderFormat, domain, action);
		return actionHanlder.get(key);
	}
}
