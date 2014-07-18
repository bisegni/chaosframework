/**
 * 
 */
package it.infn.chaos.mds.business;

import java.util.Vector;

import org.ref.common.business.BusinessObject;
import org.ref.common.business.DBColumn;
import org.ref.common.business.DBTable;
import org.ref.common.type.Timestamp;

/**
 * @author bisegni
 * 
 */
@DBTable(name = "unit_server")
public class UnitServer extends BusinessObject {
	public static final String	UNIT_SERVER_ALIAS	= "unit_server_alias";
	public static final String	UNIT_SERVER_IP_PORT	= "unit_server_ip_port";
	public static final String	UNIT_SERVER_HB_TIME	= "unit_server_hb_time";
	public static final String	CONTROL_UNIT_ALIAS	= "control_unit_alias";
	private static final long	serialVersionUID	= 947001022484682227L;
	@DBColumn(name = UNIT_SERVER_ALIAS, maxDimension = 64)
	private String				alias				= null;
	@DBColumn(name = UNIT_SERVER_IP_PORT, maxDimension = 256)
	private String				ip_port				= null;
	@DBColumn(name = UNIT_SERVER_HB_TIME, maxDimension = 256)
	private Timestamp			unitServerHB		= null;
	private Vector<String>		publischedCU		= new Vector<String>();

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
		// TODO Auto-generated method stub
		return null;
	}

	public String getAlias() {
		return alias;
	}

	public void setAlias(String alias) {
		this.alias = alias;
	}

	public String getIp_port() {
		return ip_port;
	}

	public void setIp_port(String ip_port) {
		this.ip_port = ip_port;
	}

	public Vector<String> getPublischedCU() {
		return publischedCU;
	}

	public void setPublischedCU(Vector<String> publischedCU) {
		this.publischedCU = publischedCU;
	}

	public void removePublischedCU(String publischedCU) {
		getPublischedCU().removeElement(publischedCU);
	}

	public void addPublischedCU(String publischedCU) {
		getPublischedCU().addElement(publischedCU);
	}
}
