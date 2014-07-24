/**
 * 
 */
package it.infn.chaos.mds.business;

import org.ref.common.business.BusinessObject;
import org.ref.common.business.DBColumn;
import org.ref.common.business.DBTable;

/**
 * @author bisegni
 * 
 */
@DBTable(name = "unit_server_cu_instance")
public class UnitServerCuInstance extends BusinessObject {
	private static final long	serialVersionUID	= -3540643481390952382L;
	public static final String	UNIT_SERVER_ALIAS	= "unit_server_alias";
	public static final String	CU_ID				= "cu_id";
	public static final String	CU_PARAM			= "cu_param";
	public static final String	AUTO_LOAD			= "auto_load";

	@DBColumn(name = UNIT_SERVER_ALIAS, maxDimension = 64)
	private String				unitServerAlias		= null;
	@DBColumn(name = CU_ID, maxDimension = 64)
	private String				cuId				= null;
	@DBColumn(name = CU_PARAM, maxDimension = 64)
	private String				cuParam				= null;
	@DBColumn(name = AUTO_LOAD)
	private Boolean				autoLoad			= false;

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

	public String getUnitServerAlias() {
		return unitServerAlias;
	}

	public void setUnitServerAlias(String unitServerAlias) {
		this.unitServerAlias = unitServerAlias;
	}

	public String getCuId() {
		return cuId;
	}

	public void setCuId(String cuId) {
		this.cuId = cuId;
	}

	public String getCuParam() {
		return cuParam;
	}

	public void setCuParam(String cuParam) {
		this.cuParam = cuParam;
	}

	public Boolean getAutoLoad() {
		return autoLoad;
	}

	public void setAutoLoad(Boolean autoLoad) {
		this.autoLoad = autoLoad;
	}

}
