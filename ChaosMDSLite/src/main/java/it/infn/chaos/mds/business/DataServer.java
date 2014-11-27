/**
 * 
 */
package it.infn.chaos.mds.business;

import org.bson.BasicBSONObject;
import org.ref.common.business.BusinessObject;
import org.ref.common.business.DBColumn;
import org.ref.common.business.DBPrimaryKey;
import org.ref.common.business.DBTable;

/**
 * @author bisegni
 * 
 */
@DBTable(name = "data_server")
public class DataServer extends BSONBusinessObject {
	public static final long	serialVersionUID	= -7648734249066851869L;
	public static final String	KEY_HOSTAME			= "hostname";
	public static final String	KEY_ID_SERVER		= "id_server";
	public static final String	KEY_IS_LIVE			= "is_live";
	@DBPrimaryKey
	@DBColumn(name = KEY_ID_SERVER)
	private Integer				idServer			= null;
	@DBPrimaryKey
	@DBColumn(name = KEY_HOSTAME)
	private String				hostname			= null;
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
		return String.format("%s", getHostname());
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

	/* (non-Javadoc)
	 * @see it.infn.chaos.mds.business.BSONBusinessObject#fillFromBson(java.lang.Object)
	 */
	@Override
	public void fillFromBson(Object bson) throws Throwable {
		BasicBSONObject bobj =(BasicBSONObject) bson;
		setHostname(bobj.getString(DataServer.KEY_HOSTAME));
		setIdServer(bobj.getInt(DataServer.KEY_ID_SERVER));
		setIsLiveServer(bobj.getBoolean(DataServer.KEY_IS_LIVE));
			
	}

	/* (non-Javadoc)
	 * @see it.infn.chaos.mds.business.BSONBusinessObject#toBson()
	 */
	@Override
	public Object toBson() throws Throwable {
		BasicBSONObject bobj = new BasicBSONObject();
		bobj.append(DataServer.KEY_HOSTAME, this.getHostname());
		bobj.append(DataServer.KEY_ID_SERVER, this.getIdServer());
		bobj.append(DataServer.KEY_IS_LIVE, this.getIsLiveServer());
		return bobj;
	}

}
