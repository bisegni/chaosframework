/**
 * 
 */
package it.infn.chaos.mds.business;

import org.ref.common.business.BusinessObject;
import org.ref.common.business.DBColumn;
import org.ref.common.business.DBPrimaryKey;
import org.ref.common.business.DBTable;

/**
 * @author bisegni
 * 
 */
@DBTable(name = "data_server")
public class DataServer extends BusinessObject {
	public static final long	serialVersionUID	= -7648734249066851869L;
	public static final String	KEY_PORT			= "port";
	public static final String	KEY_HOSTAME			= "hostname";
	public static final String	KEY_ID_SERVER		= "id_server";
	public static final String	KEY_IS_LIVE			= "is_live";
	@DBPrimaryKey
	@DBColumn(name = KEY_ID_SERVER)
	private Integer				idServer			= null;
	@DBPrimaryKey
	@DBColumn(name = KEY_HOSTAME)
	private String				hostname			= null;
	@DBColumn(name = KEY_PORT)
	private Integer				port				= null;
	@DBColumn(name = KEY_IS_LIVE)
	private Boolean				isLiveServer		= null;

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.ref.common.business.BusinessObject#equals(java.lang.Object)
	 */
	@Override
	public boolean equals(Object arg0) {
		// TODO Auto-generated method stub
		return false;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.ref.common.business.BusinessObject#hashCode()
	 */
	@Override
	public int hashCode() {
		// TODO Auto-generated method stub
		return 0;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.ref.common.business.BusinessObject#toString()
	 */
	@Override
	public String toString() {
		return String.format("%s:%s", getHostname(), getPort());
	}

	/**
	 * @return the idServer
	 */
	public Integer getIdServer() {
		return idServer;
	}

	/**
	 * @param idServer
	 *            the idServer to set
	 */
	public void setIdServer(Integer idServer) {
		this.idServer = idServer;
	}

	/**
	 * @return the hostname
	 */
	public String getHostname() {
		return hostname;
	}

	/**
	 * @param hostname
	 *            the hostname to set
	 */
	public void setHostname(String hostname) {
		this.hostname = hostname;
	}

	/**
	 * @return the port
	 */
	public Integer getPort() {
		return port;
	}

	/**
	 * @param port
	 *            the port to set
	 */
	public void setPort(Integer port) {
		this.port = port;
	}

	/**
	 * @return the isLiveServer
	 */
	public Boolean getIsLiveServer() {
		return isLiveServer;
	}

	/**
	 * @param isLiveServer
	 *            the isLiveServer to set
	 */
	public void setIsLiveServer(Boolean isLiveServer) {
		this.isLiveServer = isLiveServer;
	}

}
