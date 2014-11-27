/**
 * 
 */
package it.infn.chaos.mds.business;

import it.infn.chaos.mds.RPCConstants;

import java.util.Vector;

import org.bson.BSONObject;
import org.bson.BasicBSONObject;
import org.bson.types.BasicBSONList;
import org.ref.common.business.BusinessObject;
import org.ref.common.business.DBColumn;
import org.ref.common.business.DBTable;
import org.ref.common.exception.RefException;

import com.mongodb.util.JSON;

/**
 * @author bisegni
 * 
 */
@DBTable(name = "unit_server_cu_instance")
public class UnitServerCuInstance extends BSONBusinessObject {
	private static final long			serialVersionUID		= -3540643481390952382L;
	public static final String			UNIT_SERVER_ALIAS		= "unit_server_alias";
	public static final String			CU_ID					= "cu_id";
	public static final String			CU_TYPE					= "cu_type";
	public static final String			CU_PARAM				= "cu_param";
	public static final String			DRIVER_SPEC				= "driver_spec";
	public static final String			AUTO_LOAD				= "auto_load";
	public static final String			STATE					= "state";
	public static final String			CU_INTERFACE			= "cu_interface";
	public static final String			ATTR_SPEC				= "attr_spec";

	
	@DBColumn(name = UNIT_SERVER_ALIAS, maxDimension = 64)
	private String						unitServerAlias			= null;
	@DBColumn(name = CU_ID, maxDimension = 64)
	private String						cuId					= null;
	private String						oldCUId					= null;

	@DBColumn(name = CU_TYPE, maxDimension = 64)
	private String						cuType					= null;
	@DBColumn(name = DRIVER_SPEC)
	private String						drvSpec					= null;
	private Vector<DriverSpec>			driverSpec				= new Vector<DriverSpec>();

	@DBColumn(name = CU_PARAM, maxDimension = 64)
	private String						cuParam					= null;
	@DBColumn(name = AUTO_LOAD)
	private Boolean						autoLoad				= false;
	@DBColumn(name = STATE, maxDimension = 64)
	private String						state					= "----";

	private Vector<DatasetAttribute>	attributeConfiguration	= new Vector<DatasetAttribute>();

	private String classInterface=null;
	
	public Vector<DatasetAttribute> getAttributeConfigutaion() {
		return attributeConfiguration;
	}

	public void setInterface(String str){
		this.classInterface=str;
	}
	public String getInterface(){
		return this.classInterface;
	}
	public void setAttributeConfigutaion(Vector<DatasetAttribute> attr) {
		this.attributeConfiguration = attr;
	}

	public String getCuType() {
		return cuType;
	}

	public void setCuType(String cuType) {
		this.cuType = cuType;
	}

	@Override
	public void checkIntegrityValues() throws RefException {
		checkValue(getCuId(), "The Control Unit UID is mandatory");
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.ref.common.business.BusinessObject#equals(java.lang.Object)
	 */
	@Override
	public boolean equals(Object arg0) {
		UnitServerCuInstance c = (UnitServerCuInstance) arg0;
		if (c == null)
			return false;
		return c.getUnitServerAlias().equals(getUnitServerAlias()) && c.getCuId().equals(getCuId());
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
		return String.format("%s_%s", getUnitServerAlias(), getCuId());
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
		if (this.oldCUId == null) {
			// in this way if updat ethe id more times the first is backuped
			this.oldCUId = this.cuId;
		}
		this.cuId = cuId;
	}

	public boolean isCuIdModified() {
		return this.oldCUId != null;
	}
	
	public void cuIdModified(boolean b) {
		if(b== false){
			this.oldCUId = null;
		} else {
			this.oldCUId = this.cuId;
		}
		
	}
	public String getOldCUId() {
		return oldCUId;
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

	public Vector<DriverSpec> getDriverSpec() {
		return driverSpec;
	}

	public void setDriverSpec(Vector<DriverSpec> driverSpec) {
		this.driverSpec = driverSpec;
	}

	public BasicBSONObject getDriverDescriptionAsBson() {
		BasicBSONObject bobj = new BasicBSONObject();
		if (getDriverSpec().size() > 0) {
			BasicBSONList blist = new BasicBSONList();
			for (DriverSpec ds : getDriverSpec()) {
				BasicBSONObject dspec = new BasicBSONObject();
				
				dspec.append("DriverDescriptionName", ds.getDriverName());
				dspec.append("DriverDescriptionVersion", ds.getDriverVersion());
				dspec.append("DriverDescriptionInitParam", ds.getDriverInit());
				blist.add(dspec);
			}
			bobj.append("DriverDescription", blist);
		} else {
			if (getDrvSpec() != null) {
				bobj = (BasicBSONObject) JSON.parse(getDrvSpec());
			}
		}
		return bobj;
	}

	public BasicBSONObject getAttributeDescriptionAsBson() {
		BasicBSONObject bobj = new BasicBSONObject();
		BasicBSONList blist = new BasicBSONList();
		for (DatasetAttribute ds : getAttributeConfigutaion()) {		
				blist.add(ds.toBson());
		}
		bobj.append("AttrDesc", blist);
		 
		return bobj;
	}
	
	private  void fillDriverFromBSON(BasicBSONObject bobj){
		if (bobj.containsField("DriverDescription")) {
			
			BasicBSONList blist = (BasicBSONList) bobj.get("DriverDescription");
			for (Object object : blist) {
				BasicBSONObject bo = (BasicBSONObject) object;
				DriverSpec newDriverSpec = new DriverSpec();
				newDriverSpec.setDriverName(bo.getString("DriverDescriptionName"));
				newDriverSpec.setDriverVersion(bo.getString("DriverDescriptionVersion"));
				newDriverSpec.setDriverInit(bo.getString("DriverDescriptionInitParam"));
				addDrvierSpec(newDriverSpec);
			}
		}
	}
	public Vector<DriverSpec> fillDriverVectorFromBSONDescription() {
		getDriverSpec().clear();
		if (getDrvSpec() == null)
			return getDriverSpec();

		BasicBSONObject bobj = (BasicBSONObject) JSON.parse(getDrvSpec());
		fillDriverFromBSON(bobj);
		return getDriverSpec();
	}

	
	public void addDrvierSpec(DriverSpec driverSpec) {
		this.driverSpec.addElement(driverSpec);
	}

	public void clear() {
		this.driverSpec.clear();
	}

	public String getDrvSpec() {
		return drvSpec;
	}

	public void setDrvSpec(String drvSpec) {
		this.drvSpec = drvSpec;
	}

	public String getState() {
		return state;
	}

	public void setState(String state) {
		this.state = state;
	}

	public void removeAllDriverSpec() {
		this.driverSpec.removeAllElements();
		this.drvSpec = null;
		
	}

	@Override
	public void fillFromBson(Object bson) throws Throwable {
		BasicBSONObject bobj =(BasicBSONObject)bson;
		setUnitServerAlias(bobj.getString(UNIT_SERVER_ALIAS));
		setCuId(bobj.getString(CU_ID));
		setCuType(bobj.getString(CU_TYPE));
		setCuParam(bobj.getString(CU_PARAM));
		setAutoLoad(bobj.getBoolean(AUTO_LOAD));
		
		fillDriverFromBSON(bobj);
		setDrvSpec(getDriverDescriptionAsBson().toString());

		BasicBSONList blist = (BasicBSONList) bobj.get("AttrDesc");
		attributeConfiguration = new Vector<DatasetAttribute>();
		for(Object obj:blist){
			DatasetAttribute ds =new DatasetAttribute();
			ds.fillFromBson(obj);
			attributeConfiguration.add(ds);
		}
		
	}

	@Override
	public Object toBson() throws Throwable {
		BasicBSONObject bobj = new BasicBSONObject();
		
		bobj.append(UNIT_SERVER_ALIAS,this.getUnitServerAlias());
		bobj.append(CU_ID,this.getCuId());
		bobj.append(CU_TYPE,this.getCuType());
		bobj.append(CU_PARAM,this.getCuParam());
		bobj.append(AUTO_LOAD,this.getAutoLoad());
		//bobj.append(DRIVER_SPEC,this.getDriverDescriptionAsBson());
		//bobj.append(ATTR_SPEC,getAttributeDescriptionAsBson());
		bobj.putAll((BSONObject)getDriverDescriptionAsBson());
		bobj.putAll((BSONObject)getAttributeDescriptionAsBson());
		return bobj;
	}
}
