/**
 * 
 */
package it.infn.chaos.mds.business;

import java.util.ListIterator;
import java.util.Vector;

import org.bson.BasicBSONObject;
import org.bson.types.BasicBSONList;
import org.ref.common.business.*;
import org.ref.common.type.Timestamp;

/**
 * @author bisegni
 */
@DBTable(name = "dataset")
public class Dataset extends BSONBusinessObject {
	private static final long			serialVersionUID	= 1L;
	@DBPrimaryKey
	@DBColumn(name = "id_device")
	private Integer						deviceID			= null;
	@DBPrimaryKey
	@DBColumn(name = "id_dataset")
	private Integer						datasetID			= null;
	@DBColumn(name = "timestamp")
	private Timestamp						timestamp			= null;
	@DBColumn(isComplex = true)
	@DBRelationClass(	foreignKey = { "ID_DEVICE" },
						primaryKey = { "ID_DEVICE" })
	private Device						device				= null;
	@DBColumn(isComplex = true)
	@DBRelationClass(	foreignKey = { "ID_DATASET", "ID_DEVICE" },
						primaryKey = { "ID_DATASET", "ID_DEVICE" })
	private Vector<DatasetAttribute>	attributes			= new Vector<DatasetAttribute>();

	/**
	 * @return the device
	 */
	public Device getDevice() {
		return device;
	}

	/**
	 * @param device
	 *            the device to set
	 */
	public void setDevice(Device device) {
		this.device = device;
		setDeviceID(device == null ? null : device.getDeviceID());
	}

	/**
	 * @return the datasetID
	 */
	public Integer getDatasetID() {
		return datasetID;
	}

	/**
	 * @param datasetID
	 *            the datasetID to set
	 */
	public void setDatasetID(Integer datasetID) {
		this.datasetID = datasetID;
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

	/*
	 * 
	 */
	public boolean equals(Object ds) {
		if (!(ds instanceof Dataset))
			return false;
		Dataset dsCasted = (Dataset) ds;
		boolean result = dsCasted.getDeviceID() != null && deviceID != null && dsCasted.getDeviceID().equals(deviceID);
		result = result && dsCasted.getDatasetID() != null && datasetID != null && dsCasted.getDatasetID().equals(datasetID);
		return result;
	}

	/*
	 * (non-Javadoc)
	 * @see org.ref.common.business.BusinessObject#hashCode()
	 */
	public int hashCode() {
		return 0;
	}

	/*
	 * (non-Javadoc)
	 * @see org.ref.common.business.BusinessObject#toString()
	 */
	public String toString() {
		return "";
	}

	/**
	 * @return the attributes
	 */
	public Vector<DatasetAttribute> getAttributes() {
		return attributes;
	}

	/**
	 * @param attributes
	 *            the attributes to set
	 */
	public void setAttributes(Vector<DatasetAttribute> attributes) {
		this.attributes = attributes;
	}

	/*
	 * (non-Javadoc)
	 * @see
	 * it.infn.chaos.mds.business.BSONBusinessObject#fillFromBson(java.lang.
	 * Object)
	 */
	@Override
	public void fillFromBson(Object bson) throws Throwable {
		if (!(bson instanceof BasicBSONList))
			return;
		BasicBSONList bsonAttributesArray = (BasicBSONList) bson;
		ListIterator<Object> dsDescIter = bsonAttributesArray.listIterator();
		while (dsDescIter.hasNext()) {
			BasicBSONObject dsAttr = (BasicBSONObject) dsDescIter.next();
			DatasetAttribute dsAttrObj = new DatasetAttribute();
			dsAttrObj.fillFromBson(dsAttr);
			dsAttrObj.setCheck(dsAttrObj.toBsonHash());
			getAttributes().add(dsAttrObj);
		}
	}

	/*
	 * (non-Javadoc)
	 * @see it.infn.chaos.mds.business.BSONBusinessObject#toBson()
	 */
	@Override
	public Object toBson() {
		BasicBSONList bsonAttributesArray = new BasicBSONList();
		for (DatasetAttribute attribute : getAttributes()) {
			bsonAttributesArray.add(attribute.toBson());
		}
		return bsonAttributesArray;
	}

	/**
	 * @return the timestamp
	 */
	public Timestamp getTimestamp() {
		return timestamp;
	}

	/**
	 * @param timestamp
	 *            the timestamp to set
	 */
	public void setTimestamp(Timestamp timestamp) {
		this.timestamp = timestamp;
	}

}
