package it.infn.chaos.mds.business;


import it.infn.chaos.mds.RPCConstants;

import org.bson.BasicBSONObject;
import org.ref.common.business.*;
import org.ref.common.type.Timestamp;


@DBTable(name="device_class")
public class DeviceClass extends BSONBusinessObject {
	public static final long serialVersionUID = 1L;
	public static final String DEVICE_CLASS_NAME="device_class";
	public static final String DEVICE_CLASS_ALIAS="device_class_alias";
	public static final String DEVICE_CLASS_INTERFACE="device_class_interface";
	public static final String DEVICE_CLASS_SOURCE="device_source";
	@DBPrimaryKey
	@DBColumn(name = "id_device_class")
	private Integer classId = null;

	@DBPrimaryKey
	@DBColumn(name = DEVICE_CLASS_NAME)
	private String deviceClass = null;
	
	@DBColumn(name=DEVICE_CLASS_ALIAS)
	private String deviceClassAlias=null;

	@DBColumn(name=DEVICE_CLASS_INTERFACE)
	private String deviceClassInterface=null;

	@DBColumn(name=DEVICE_CLASS_SOURCE)
	private String deviceSource= null;

	public DeviceClass() {
	}

	public Integer getClassId() {
		return this.classId;
	}

	public void setClassId(Integer id) {
		this.classId= id;
	}

	public String getDeviceClass() {
		return this.deviceClass;
	}

	public void setDeviceClass(String deviceClass) {
		this.deviceClass = deviceClass;
	}
	
	public String getDeviceClassAlias() {
		return this.deviceClassAlias;
	}

	public void setDeviceClassAlias(String deviceClassAlias) {
		this.deviceClassAlias = deviceClassAlias;
	}

	public String getDeviceClassInterface() {
		return this.deviceClassInterface;
	}

	public void setDeviceClassInterface(String deviceClassInterface) {
		this.deviceClassInterface = deviceClassInterface;
	}

	public String getDeviceSource() {
		return this.deviceSource;
	}

	public void setDeviceSource(String deviceSource) {
		this.deviceSource = deviceSource;
	}

	/*
	 * (non-Javadoc)
	 * @see org.ref.common.business.BusinessObject#hashCode()
	 */
	@Override
	public int hashCode() {
		return 0;
	}

	/*
	 * (non-Javadoc)
	 * @see org.ref.common.business.BusinessObject#toString()
	 */
	@Override
	public String toString() {
		return null;
	}

	@Override
	public void fillFromBson(Object bson) throws Throwable {
		BasicBSONObject bobj = (BasicBSONObject) bson;
		setDeviceClassAlias(bobj.getString(DeviceClass.DEVICE_CLASS_ALIAS));
		setDeviceClass(bobj.getString(DeviceClass.DEVICE_CLASS_NAME));
		setDeviceClassInterface(bobj.getString(DeviceClass.DEVICE_CLASS_INTERFACE));
		setDeviceSource(bobj.getString(DeviceClass.DEVICE_CLASS_SOURCE));
	}

	@Override
	public Object toBson() throws Throwable {
		BasicBSONObject bobj = new BasicBSONObject();
		bobj.append(DeviceClass.DEVICE_CLASS_ALIAS, this.getDeviceClassAlias());
		bobj.append(DeviceClass.DEVICE_CLASS_NAME, this.getDeviceClass());
		bobj.append(DeviceClass.DEVICE_CLASS_INTERFACE, this.getDeviceClassInterface());
		bobj.append(DeviceClass.DEVICE_CLASS_SOURCE, this.getDeviceSource());
		return bobj;
	}
	
	@Override
	public boolean equals(Object d) {
		if (!(d instanceof Device))
			return false;
		return classId != null && (((DeviceClass) d).getClassId() != null) && ((Device) d).getDeviceID().equals(classId);
	}
}