/**
 * 
 */
package it.infn.chaos.mds.business;

import java.util.Vector;

import org.bson.BasicBSONObject;
import org.bson.types.BasicBSONList;
import org.ref.common.business.BusinessObject;
import org.ref.common.business.DBColumn;
import org.ref.common.business.DBTable;
import org.ref.common.type.Timestamp;

/**
 * @author bisegni
 * 
 */
@DBTable(name = "unit_server")
public class UnitServer extends BSONBusinessObject {
	public boolean isAliasChanged() {
		return isAliasChanged;
	}

	public static final String	UNIT_SERVER_ALIAS	= "unit_server_alias";
	public static final String	UNIT_SERVER_IP_PORT	= "unit_server_ip_port";
	public static final String	UNIT_SERVER_HB_TIME	= "unit_server_hb_time";
	public static final String	CONTROL_UNIT_ALIAS	= "control_unit_alias";
	public static final String	PRIVATE_KEY			= "private_key";
	public static final String	PUBLIC_KEY			= "public_key";
	public static final String	REG_STATE			= "reg_state";

	private static final long	serialVersionUID	= 947001022484682227L;

	private String				oldAliasOnChange	= null;

	public String getOldAliasOnChange() {
		return oldAliasOnChange;
	}

	@DBColumn(name = UNIT_SERVER_ALIAS, maxDimension = 64)
	private String			alias			= null;
	@DBColumn(name = UNIT_SERVER_IP_PORT, maxDimension = 256)
	private String			ip_port			= null;
	@DBColumn(name = UNIT_SERVER_HB_TIME, maxDimension = 256)
	private Timestamp		unitServerHB	= null;
	@DBColumn(name = PUBLIC_KEY, maxDimension = 1024)
	private String			public_key		= null;
	@DBColumn(name = PRIVATE_KEY, maxDimension = 1024)
	private String			private_key		= null;

	@DBColumn(name = REG_STATE)
	private Integer			reg_state		= null;
	
	private Vector<String>	publischedCU	= new Vector<String>();
	private boolean			isAliasChanged	= false;
	private Vector<UnitServerCuInstance>	cuInstances	= new Vector<UnitServerCuInstance>();

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
		isAliasChanged = true;
		this.oldAliasOnChange = this.alias;
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

	public Timestamp getUnitServerHB() {
		return unitServerHB;
	}

	public void setUnitServerHB(Timestamp unitServerHB) {
		this.unitServerHB = unitServerHB;
	}

	public String getPublic_key() {
		return public_key;
	}

	public void setPublic_key(String public_key) {
		this.public_key = public_key;
	}

	public String getPrivate_key() {
		return private_key;
	}

	public void setPrivate_key(String private_key) {
		this.private_key = private_key;
	}
	
	public Integer getState() {
		return this.reg_state;
	}

	public void setState(Integer stat) {
		this.reg_state = stat;
	}

	public void setCuInstances(Vector<UnitServerCuInstance> cuiv){
		this.cuInstances = cuiv;
	}
	public Vector<UnitServerCuInstance>  getCuInstances(){
		return this.cuInstances;
	}
	
	@Override
	public void fillFromBson(Object bson) throws Throwable {
		BasicBSONObject bobj =(BasicBSONObject) bson;
		this.setAlias(bobj.getString(UNIT_SERVER_ALIAS));
		this.setPrivate_key(bobj.getString(PRIVATE_KEY));
		this.setPrivate_key(bobj.getString(PUBLIC_KEY));
		BasicBSONList blist = (BasicBSONList)bobj.get("cu_desc");
		cuInstances = new Vector<UnitServerCuInstance>();
		for (Object object : blist) {
			UnitServerCuInstance cu = new UnitServerCuInstance();
			BasicBSONObject bo = (BasicBSONObject) object;
			cu.fillFromBson(bo);
			cuInstances.add(cu);
		}
		
	}

	@Override
	public Object toBson() throws Throwable {
		BasicBSONObject bobj = new BasicBSONObject();
		bobj.append(UNIT_SERVER_ALIAS, this.getAlias());
		bobj.append(PRIVATE_KEY, this.getPrivate_key());
		bobj.append(PUBLIC_KEY, this.getPublic_key());
		BasicBSONList blist = new BasicBSONList();
		for (UnitServerCuInstance ds : cuInstances) {
				blist.add(ds.toBson());
		}
		
		bobj.append("cu_desc", blist);
		return bobj;
	}
}
