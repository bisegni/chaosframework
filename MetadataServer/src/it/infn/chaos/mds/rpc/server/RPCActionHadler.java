/**
 * 
 */
package it.infn.chaos.mds.rpc.server;

import java.sql.Connection;
import java.sql.SQLException;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;

import org.bson.BasicBSONObject;
import org.ref.common.exception.RefException;
import org.ref.server.data.DataAccess;
import org.ref.server.db.ConnectionPoolManager;

/**
 * @author bisegni
 */
abstract public class RPCActionHadler {
	private Connection							connection	= null;
	private Hashtable<String, DomainActions>	domainHash	= new Hashtable<String, DomainActions>();

	/**
	 * Initialization method where an hablder must register the domain and action alias for witch it
	 * can respond
	 * 
	 * @throws RefException
	 */
	abstract public void intiHanlder() throws RefException;

	/**
	 * @param domain
	 * @param action
	 * @param actionData
	 */
	abstract public BasicBSONObject handleAction(String domain, String action, BasicBSONObject actionData) throws Throwable;

	/**
	 * @param domain
	 * @param action
	 * @param actionData
	 */
	public BasicBSONObject _handleAction(String domain, String action, BasicBSONObject actionData) {
		BasicBSONObject result = null;
		try {
			result = handleAction(domain, action, actionData);
		} catch (RefException e) {
			result = actionData;
			RPCUtils.addRefExceptionToBson(result, e);
		} catch (Throwable e) {
			result = actionData;
			RPCUtils.addRefExceptionElementToBson(result, "CURegistration::getLastDataset", e.getMessage(), 0);
		}
		return result;
	}

	/**
	 * @param mdsRpcServer
	 * @throws RefException
	 */
	public void registerDomanAndNameForHandler(RPCServer mdsRpcServer) throws RefException {
		if (mdsRpcServer == null)
			return;

		Enumeration<DomainActions> actions = domainHash.elements();
		while (actions.hasMoreElements()) {
			RPCActionHadler.DomainActions domainActions = (RPCActionHadler.DomainActions) actions.nextElement();
			Vector<String> acts = domainActions.getNames();
			for (String actionAlias : acts) {
				mdsRpcServer.addDomainActionHanlder(domainActions.getDomain(), actionAlias, this);
			}
		}

	}

	/**
	 * @param dName
	 * @return
	 */
	protected DomainActions getDomainDescriptionByName(String dName) {
		DomainActions result = domainHash.get(dName);
		if (result == null) {
			result = new DomainActions(dName);
			domainHash.put(dName, result);
		}
		return result;
	}

	/**
	 * @param domain
	 * @param alias
	 */
	protected void addDomainAction(String domain, String alias) {
		getDomainDescriptionByName(domain).getNames().add(alias);
	}

	/**
	 * @return the connection
	 */
	public Connection getConnection() {
		return connection;
	}

	/**
	 * @param connection
	 *            the connection to set
	 */
	public void setConnection(Connection connection) {
		this.connection = connection;
	}

	/**
	 * @param daClass
	 * @return
	 * @throws InstantiationException
	 * @throws IllegalAccessException
	 * @throws SQLException
	 */
	protected <T extends DataAccess> T getDataAccessInstance(Class<T> daClass) throws InstantiationException, IllegalAccessException, SQLException {
		T result = daClass.newInstance();
		// add a new conneciton
		result.setConnection(ConnectionPoolManager.getInstace().getPooledConnection());
		return result;
	}

	/**
	 * @param daToClose
	 * @param commit
	 * @throws SQLException
	 */
	protected void closeDataAccess(DataAccess daToClose, boolean commit) throws SQLException {
		if (commit)
			daToClose.getConnection().commit();
		else
			daToClose.getConnection().rollback();
		daToClose.getConnection().close();
	}

	/**
	 * @author bisegni
	 */
	class DomainActions {
		private String			domain	= null;
		private Vector<String>	names	= new Vector<String>();

		public DomainActions(String domain) {
			this.domain = domain;
		}

		/**
		 * @return the domain
		 */
		public String getDomain() {
			return domain;
		}

		/**
		 * @param domain
		 *            the domain to set
		 */
		public void setDomain(String domain) {
			this.domain = domain;
		}

		/**
		 * @return the names
		 */
		public Vector<String> getNames() {
			return names;
		}
	}
}
