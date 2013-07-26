/**
 * 
 */
package it.infn.chaos.mds.business;

import it.infn.chaos.mds.RPCConstants;

import org.bson.BasicBSONObject;
import org.ref.common.business.*;
import org.ref.common.type.Timestamp;

/**
 * @author bisegni
 */
@DBTable(name = "devices")
public class Device extends BSONBusinessObject {
	public static final String	INIT_AT_STARTUP			= "init_at_startup";
	public static final String	LAST_HB					= "last_hb";
	public static final String	ID_DEVICE				= "id_device";
	public static final String	NET_ADDRESS				= "net_address";
	public static final String	CU_INSTANCE				= "cu_instance";
	public static final String	DEVICE_IDENTIFICATION	= "device_identification";
	private static final long	serialVersionUID		= 6502276887373714133L;
	@DBColumn(	name = DEVICE_IDENTIFICATION,
				maxDimension = 40)
	private String				deviceIdentification	= null;
	@DBColumn(	name = CU_INSTANCE,
				maxDimension = 40)
	private String				cuInstance				= null;
	@DBColumn(	name = NET_ADDRESS,
				maxDimension = 40)
	private String				netAddress				= null;
	@DBPrimaryKey
	@DBColumn(name = ID_DEVICE)
	private Integer				deviceID				= null;
	@DBColumn(isComplex = true)
	private Dataset				dataset					= null;
	@DBColumn(name = LAST_HB)
	private Timestamp			lastHeartBeatTimestamp	= null;
	@DBColumn(name = INIT_AT_STARTUP)
	private Boolean				initAtStartup			= false;

	/*
	 * (non-Javadoc)
	 * @see org.ref.common.business.BusinessObject#equals(java.lang.Object)
	 */
	@Override
	public boolean equals(Object d) {
		if (!(d instanceof Device))
			return false;
		return deviceID != null && ((Device) d).getDeviceID() != null && ((Device) d).getDeviceID().equals(deviceID);
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

	/**
	 * @return the deviceID
	 */
	public Integer getDeviceID() {
		return deviceID;
	}

	/**
	 * @param deviceID
	 *            the deviceID to set
	 */
	public void setDeviceID(Integer deviceID) {
		this.deviceID = deviceID;
	}

	/**
	 * @return the cuInstance
	 */
	public String getCuInstance() {
		return cuInstance;
	}

	/**
	 * @param cuInstance
	 *            the cuInstance to set
	 */
	public void setCuInstance(String cuInstance) {
		this.cuInstance = cuInstance;
	}

	/**
	 * @return the dataset
	 */
	public Dataset getDataset() {
		return dataset;
	}

	/**
	 * @param dataset
	 *            the dataset to set
	 */
	public void setDataset(Dataset dataset) {
		this.dataset = dataset;
	}

	/**
	 * @return the deviceIdentification
	 */
	public String getDeviceIdentification() {
		return deviceIdentification;
	}

	/**
	 * @param deviceIdentification
	 *            the deviceIdentification to set
	 */
	public void setDeviceIdentification(String deviceIdentification) {
		this.deviceIdentification = deviceIdentification;
	}

	/**
	 * @return the netAddress
	 */
	public String getNetAddress() {
		return netAddress;
	}

	/**
	 * @param netAddress
	 *            the netAddress to set
	 */
	public void setNetAddress(String netAddress) {
		this.netAddress = netAddress;
	}

	@Override
	public Object toBson() {
		BasicBSONObject deviceObj = new BasicBSONObject();
		if (getDeviceIdentification() != null)
			deviceObj.append(RPCConstants.DATASET_DEVICE_ID, getDeviceIdentification());
		if (getDataset() != null)
			deviceObj.append(RPCConstants.DATASET_DESCRIPTION, getDataset().toBson());
		return deviceObj;
	}

	/**
	 * @throws Throwable
	 */
	public void fillFromBson(Object bson) throws Throwable {
		if (!(bson instanceof BasicBSONObject))
			return;
		BasicBSONObject bbObject = (BasicBSONObject) bson;
		if (bbObject.containsField(RPCConstants.DATASET_DEVICE_ID)) {
			setDeviceIdentification(bbObject.getString(RPCConstants.DATASET_DEVICE_ID));
		}
		if (bbObject.containsField(RPCConstants.DATASET_DESCRIPTION)) {
			// there is the description of the dataset
			Dataset ds = new Dataset(bbObject.containsField(RPCConstants.DATASET_TIMESTAMP)?bbObject.getLong(RPCConstants.DATASET_TIMESTAMP):null);
			ds.setDevice(this);
			ds.fillFromBson(bbObject.get(RPCConstants.DATASET_DESCRIPTION));
			setDataset(ds);
		}

	}

	/**
	 * @return the lastHeartBeatTimestamp
	 */
	public Timestamp getLastHeartBeatTimestamp() {
		return lastHeartBeatTimestamp;
	}

	/**
	 * @param lastHeartBeatTimestamp
	 *            the lastHeartBeatTimestamp to set
	 */
	public void setLastHeartBeatTimestamp(Timestamp lastHeartBeatTimestamp) {
		this.lastHeartBeatTimestamp = lastHeartBeatTimestamp;
	}

	/**
	 * @return the initAtStartup
	 */
	public Boolean getInitAtStartup() {
		return initAtStartup;
	}

	/**
	 * @param initAtStartup
	 *            the initAtStartup to set
	 */
	public void setInitAtStartup(Boolean initAtStartup) {
		this.initAtStartup = initAtStartup;
	}

}
