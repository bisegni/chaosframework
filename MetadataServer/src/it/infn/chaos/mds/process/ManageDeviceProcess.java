/**
 * 
 */
package it.infn.chaos.mds.process;

import it.infn.chaos.mds.business.Dataset;
import it.infn.chaos.mds.business.DatasetAttribute;
import it.infn.chaos.mds.business.Device;
import it.infn.chaos.mds.da.DeviceDA;

import java.util.Collection;
import java.util.List;

import org.ref.server.interapplicationenvironment.ProcessActionDescription;
import org.ref.server.interapplicationenvironment.ProcessDescription;
import org.ref.server.process.RefProcess;

/**
 * @author bisegni
 */
@ProcessDescription
@SuppressWarnings("serial")
public class ManageDeviceProcess extends RefProcess {

	@ProcessActionDescription
	public void getFullDevicDeskOrInsert() {

	}

	/**
	 * return all the registered device
	 * 
	 * @return
	 * @throws Throwable
	 */
	@ProcessActionDescription
	public List<Device> getAlDevices() throws Throwable {
		DeviceDA dDA = (DeviceDA) getDataAccessInstance(DeviceDA.class);
		return dDA.getAllDevice();
	}

	/**
	 * return the startup initialization feature of device
	 * 
	 * @param deviceIdentification
	 * @return
	 * @throws Throwable
	 */
	public boolean isDeviceInitializableAtStartup(String deviceIdentification) throws Throwable {
		DeviceDA dDA = (DeviceDA) getDataAccessInstance(DeviceDA.class);
		Device d = dDA.getDeviceFromDeviceIdentification(deviceIdentification);
		return d!=null?d.getInitAtStartup():false;
	}

	/**
	 * 
	 * @param deviceIdentification
	 * @param initialized
	 * @throws Exception 
	 */
	public void setDeviceStartupInitizializationOption(String deviceIdentification, boolean init) throws Exception {
		try {
			DeviceDA dDA = (DeviceDA) getDataAccessInstance(DeviceDA.class);
			dDA.setStartupInitializationOption(deviceIdentification, init);
			commit();
		} catch (Exception e) {
			rollback();
			throw e;
		}
	}

	/**
	 * 
	 * @param deviceIdentification
	 * @throws Throwable
	 */
	public void swapDeviceStartupInitizializationOption(String deviceIdentification) throws Throwable {
		DeviceDA dDA = (DeviceDA) getDataAccessInstance(DeviceDA.class);
		Device d = dDA.getDeviceFromDeviceIdentification(deviceIdentification);
		setDeviceStartupInitizializationOption(d.getDeviceIdentification(), !d.getInitAtStartup());
	}
	
	/**
	 * @param d
	 * @return
	 * @throws Throwable
	 */
	public List<Dataset> getAllDatasetForDevice(Device d) throws Throwable {
		return getAllDatasetForDeviceIdentification(d.getDeviceIdentification());
	}

	/**
	 * @param deviceIdentification
	 * @return
	 * @throws Throwable
	 */
	public List<Dataset> getAllDatasetForDeviceIdentification(String deviceIdentification) throws Throwable {
		DeviceDA dDA = (DeviceDA) getDataAccessInstance(DeviceDA.class);
		return dDA.getDatasetsForDeviceIndetification(deviceIdentification);
	}

	/**
	 * Return the dataset attribute list for the dataset
	 * 
	 * @param dsAttr
	 * @return
	 * @throws Throwable
	 */
	public List<DatasetAttribute> getAttributeForDataset(Dataset dsAttr) throws Throwable {
		DeviceDA dDA = (DeviceDA) getDataAccessInstance(DeviceDA.class);
		return dDA.getDatasetsAttributeForDataset(dsAttr.getDeviceID(), dsAttr.getDatasetID());
	}

	/**
	 * 
	 * @param datasetAttributes
	 * @throws Exception
	 */
	public void updateDeviceAttributes(Collection<DatasetAttribute> datasetAttributes) throws Exception {
		DeviceDA dDA = (DeviceDA) getDataAccessInstance(DeviceDA.class);
		try {
			for (DatasetAttribute datasetAttribute : datasetAttributes) {
				dDA.updateDatasetAttribute(datasetAttribute);
			}
			commit();
		} catch (Exception e) {
			rollback();
			throw e;
		}

	}

	/**
	 * Delete device by device identification
	 * 
	 * @param string
	 * @throws Throwable
	 */
	public void deleteDeviceByInstance(String deviceIdentification) throws Throwable {
		DeviceDA dDA = (DeviceDA) getDataAccessInstance(DeviceDA.class);
		try {
			dDA.deleteDeviceByDeviceIdentification(deviceIdentification);
			commit();
		} catch (Exception e) {
			rollback();
			throw e;
		}
	}



}
