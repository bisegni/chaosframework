/**
 * 
 */
package it.infn.chaos.mds.da;

import it.infn.chaos.mds.business.Dataset;
import it.infn.chaos.mds.business.DatasetAttribute;
import it.infn.chaos.mds.business.Device;

import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.Date;
import java.util.List;
import java.util.Vector;

import org.ref.server.data.DataAccess;
import org.ref.server.db.sqlbuilder.DeleteSqlBuilder;
import org.ref.server.db.sqlbuilder.InsertUpdateBuilder;
import org.ref.server.db.sqlbuilder.SqlBuilder;
import org.ref.server.db.sqlbuilder.SqlTable;

/**
 * @author bisegni
 */
public class DeviceDA extends DataAccess {

	/**
	 * Performe the device heartbeat
	 * 
	 * @param deviceID
	 * @throws SQLException
	 */
	public void performDeviceHB(Integer deviceID) throws SQLException {
		InsertUpdateBuilder iuBuilder = new InsertUpdateBuilder(InsertUpdateBuilder.MODE_UPDATE);
		iuBuilder.addTable(Device.class);
		iuBuilder.addColumnAndValue("last_hb", "$CURRENT_TIMESTAMP");
		iuBuilder.addCondition(true, "id_device=?");
		PreparedStatement ps = getPreparedStatementForSQLCommand(iuBuilder.toString());
		int idx = iuBuilder.fillPreparedStatement(ps);
		ps.setInt(idx++, deviceID);
		executeInsertUpdateAndClose(ps);
	}

	/**
	 * @param deviceIdentification
	 * @return
	 * @throws Exception
	 */
	public boolean isDeviceToBeInitialized(String deviceIdentification) throws Exception {
		boolean result = false;
		ResultSet rs = null;
		PreparedStatement ps = null;
		SqlBuilder sql = new SqlBuilder();
		SqlTable t = sql.addTable(Device.class);
		sql.addTableColumnToSelect(t.getTableName(), "init_at_startup");
		sql.addCondition(true, "device_identification = ?");
		try {
			ps = getPreparedStatementForSQLCommand(sql.toString());
			ps.setString(1, deviceIdentification);
			rs = ps.executeQuery();
			result = rs.next() ? rs.getBoolean(1) : false;
		} catch (Exception e) {
			throw e;
		} finally {
			closeResultSet(rs);
			closePreparedStatement(ps);
		}

		return result;
	}

	/**
	 * @param deviceIdentification
	 * @param init
	 * @throws SQLException
	 */
	public void setStartupInitializationOption(String deviceIdentification, boolean init) throws SQLException {
		InsertUpdateBuilder update = new InsertUpdateBuilder(InsertUpdateBuilder.MODE_UPDATE);
		update.addTable(Device.class);
		update.addColumnAndValue(Device.INIT_AT_STARTUP, init);
		update.addCondition(true, String.format("%s=?", Device.DEVICE_IDENTIFICATION));
		PreparedStatement ps = getPreparedStatementForSQLCommand(update.toString());
		Integer idx = update.fillPreparedStatement(ps);
		ps.setString(idx++, deviceIdentification);
		executeInsertUpdateAndClose(ps);
	}

	/**
	 * Performe the device heartbeat
	 * 
	 * @param deviceID
	 * @throws Throwable
	 */
	public void performDeviceHB(String deviceIdentifiation) throws Throwable {
		performDeviceHB(getDeviceIdFormInstance(deviceIdentifiation));
	}

	/**
	 * @param deviceIdentifiation
	 * @return
	 * @throws Throwable
	 */
	public Date getLastHeatbeat(String deviceIdentifiation) throws Throwable {
		return getLastHeatbeat(getDeviceIdFormInstance(deviceIdentifiation));
	}

	/**
	 * @param deviceIdentifiation
	 * @return
	 * @throws Exception
	 */
	public Date getLastHeatbeat(Integer deviceID) throws Exception {
		ResultSet rs = null;
		PreparedStatement ps = null;
		Date tsResult = null;
		try {
			SqlBuilder devSB = new SqlBuilder();
			SqlTable t = devSB.addTable(Device.class);
			devSB.addTableColumnToSelect(t.getTableName(), "last_hb");
			devSB.addCondition(true, "id_device=?");
			ps = getPreparedStatementForSQLCommand(devSB.toString());
			ps.setInt(1, deviceID);
			rs = ps.executeQuery();
			tsResult = new Date(rs.next() ? rs.getTimestamp(1).getTime() : null);
		} catch (Exception e) {
			throw e;
		} finally {
			closeResultSet(rs);
			closePreparedStatement(ps);
		}
		return tsResult;
	}

	/**
	 * return all the device
	 * 
	 * @return
	 * @throws Throwable
	 */
	@SuppressWarnings("unchecked")
	public List<Device> getAllDevice() throws Throwable {
		SqlBuilder devSB = new SqlBuilder();
		SqlTable t = devSB.addTable(Device.class);
		devSB.addTableColumnToSelect(t.getTableName(), "*");
		devSB.addFooter("order by device_identification");
		PreparedStatement ps = getPreparedStatementForSQLCommand(devSB.toString());
		return (List<Device>) executeQueryPreparedStatementAndClose(ps, Device.class, null, null, false);
	}

	/**
	 * @return
	 * @throws Throwable
	 */
	public List<Device> getAllActiveDevice() throws Throwable {
		return getAllDevice();

	}

	/**
	 * @param deviceIdentification
	 * @throws Throwable
	 */
	public void deleteDeviceByDeviceIdentification(String deviceIdentification) throws Throwable {
		deleteDeviceByDeviceID(getDeviceIdFormInstance(deviceIdentification));
	}

	/**
	 * @param deviceIdentification
	 * @throws SQLException
	 */
	public void deleteDeviceByDeviceID(Integer idDevice) throws SQLException {
		deleteDatasetsForDeviceID(idDevice);
		DeleteSqlBuilder dSqlBuilder = new DeleteSqlBuilder();
		dSqlBuilder.addTable(Device.class);
		dSqlBuilder.addCondition(true, "id_device = ?");
		PreparedStatement ps = getPreparedStatementForSQLCommand(dSqlBuilder.toString());
		ps.setInt(1, idDevice);
		executeInsertUpdateAndClose(ps);
	}

	/**
	 * @param deviceInstance
	 * @return
	 * @throws Throwable
	 */
	public Integer getDeviceIdFormInstance(String deviceInstance) throws Throwable {
		SqlBuilder devSB = new SqlBuilder();
		SqlTable t = devSB.addTable(Device.class);
		devSB.addTableColumnToSelect(t.getTableName(), "id_device");
		devSB.addCondition(true, "device_identification = ?");

		Integer result = null;
		ResultSet rs = null;
		PreparedStatement ps = null;
		try {
			ps = getPreparedStatementForSQLCommand(devSB.toString());
			ps.setString(1, deviceInstance);
			rs = ps.executeQuery();
			result = rs.next() ? rs.getInt(1) : null;
		} catch (Throwable e) {
			throw e;
		} finally {
			closeResultSet(rs);
			closePreparedStatement(ps);
		}

		return result;
	}

	/**
	 * @throws Throwable
	 */
	public Device getDeviceFromDeviceIdentification(String deviceIdentification) throws Throwable {
		Device result = null;
		SqlBuilder devSB = new SqlBuilder();
		SqlTable t = devSB.addTable(Device.class);
		devSB.addTableColumnToSelect(t.getTableName(), "*");
		devSB.addCondition(true, "device_identification = ?");
		PreparedStatement ps = getPreparedStatementForSQLCommand(devSB.toString());
		ps.setString(1, deviceIdentification);
		result = (Device) executeQuerySingleResultPreparedStatementAndClose(ps, Device.class, null, null, false);
		result.setDataset(getLastDatasetForDeviceIdentification(deviceIdentification));
		return result;
	}

	/**
	 * Insert a new device
	 * 
	 * @param d
	 * @throws Exception
	 */
	public Device insertDevice(Device d) throws Exception {
		InsertUpdateBuilder iuBuilder = new InsertUpdateBuilder();
		iuBuilder.addColumnAndValue("last_hb", "$CURRENT_TIMESTAMP");
		iuBuilder.fillWithBusinessClass(d);
		PreparedStatement ps = getPreparedStatementForInputUpdateBuilder(iuBuilder);
		iuBuilder.fillPreparedStatement(ps);
		executeInsertUpdateAndClose(ps);
		d.setDeviceID(getLastID());

		if (d.getDataset() != null) {
			d.getDataset().setDeviceID(d.getDeviceID());
			insertNewDataset(d.getDataset());
		}
		return d;
	}

	/**
	 * Update the CU instance ofr device id
	 * 
	 * @throws SQLException
	 */
	public void updateCUInstanceAndAddressForDeviceID(String deviceIdentification, String cuInstance, String netAddress) throws SQLException {
		InsertUpdateBuilder iuBuilder = new InsertUpdateBuilder(InsertUpdateBuilder.MODE_UPDATE);
		iuBuilder.addTable(Device.class);
		iuBuilder.addColumnAndValue("cu_instance", cuInstance);
		iuBuilder.addColumnAndValue("net_address", netAddress);
		iuBuilder.addCondition(true, "device_identification = ?");
		PreparedStatement ps = getPreparedStatementForSQLCommand(iuBuilder.toString());
		int idx = iuBuilder.fillPreparedStatement(ps);
		ps.setString(idx++, deviceIdentification);
		executeInsertUpdateAndClose(ps);
	}

	/**
	 * Chekc the presence of the device id instance
	 * 
	 * @return
	 * @throws Exception
	 */
	public boolean isDeviceIDPresent(String deviceInstanceID) throws Exception {
		SqlBuilder sbBuilder = new SqlBuilder();
		sbBuilder.addTable(Device.class);
		sbBuilder.addPseudoColumntoSelect("count(*)");
		sbBuilder.addCondition(true, "device_identification = ?");

		boolean result = false;
		ResultSet rs = null;
		PreparedStatement ps = null;
		try {
			ps = getPreparedStatementForSQLCommand(sbBuilder.toString());
			ps.setString(1, deviceInstanceID);
			rs = ps.executeQuery();
			result = rs.next() ? rs.getInt(1) > 0 : false;
		} catch (Exception e) {
			throw e;
		} finally {
			closeResultSet(rs);
			closePreparedStatement(ps);
		}
		return result;
	}

	/**
	 * return all datasets for the devices
	 * 
	 * @param deviceIdentification
	 * @return
	 * @throws Throwable
	 */
	@SuppressWarnings("unchecked")
	public List<Dataset> getDatasetsForDeviceIndetification(String deviceIdentification) throws Throwable {
		SqlBuilder dsBuilder = new SqlBuilder();
		SqlTable tds = dsBuilder.addTable(Dataset.class).setAlias("ds");
		dsBuilder.addTable(Device.class).setAlias("d");
		dsBuilder.addTableColumnToSelect(tds.getTableName(), "*");
		dsBuilder.addCondition(true, "ds.id_device = d.id_device");
		dsBuilder.addCondition(true, "d.device_identification = ?");
		dsBuilder.addFooter("order by ds.timestamp desc");
		PreparedStatement ps = getPreparedStatementForSQLCommand(dsBuilder.toString());
		ps.setString(1, deviceIdentification);
		return (List<Dataset>) executeQueryPreparedStatementAndClose(ps, Dataset.class, null, null, false);
	}

	/**
	 * @param deviceIdentification
	 * @throws SQLException
	 */
	public void deleteDatasetsForDeviceID(Integer idDevice) throws SQLException {
		// delete first all attribute
		deleteDatasetAttributeForDeviceID(idDevice);

		DeleteSqlBuilder dSqlBuilder = new DeleteSqlBuilder();
		dSqlBuilder.addTable(Dataset.class);
		dSqlBuilder.addCondition(true, "id_device = ?");
		PreparedStatement ps = getPreparedStatementForSQLCommand(dSqlBuilder.toString());
		ps.setInt(1, idDevice);
		executeInsertUpdateAndClose(ps);

	}

	/**
	 * Get the newest dataset for the device instance
	 * 
	 * @param deviceInstance
	 * @return
	 * @throws Throwable
	 */
	public Dataset getLastDatasetForDeviceIdentification(String deviceIdentification) throws Throwable {
		SqlBuilder dsBuilder = new SqlBuilder();
		SqlTable tds = dsBuilder.addTable(Dataset.class).setAlias("ds");
		dsBuilder.addTable(Device.class).setAlias("d");
		dsBuilder.addTableColumnToSelect(tds.getTableName(), "*");
		dsBuilder.addCondition(true, "ds.timestamp = (select max(timestamp) from dataset where id_device = d.id_device)");
		dsBuilder.addCondition(true, "ds.id_device = d.id_device");
		dsBuilder.addCondition(true, "d.device_identification = ?");
		PreparedStatement ps = getPreparedStatementForSQLCommand(dsBuilder.toString());
		ps.setString(1, deviceIdentification);
		return (Dataset) executeQuerySingleResultPreparedStatementAndClose(ps, Dataset.class, null, null, false);
	}

	/**
	 * @param ds
	 * @throws Exception
	 */
	public Dataset insertNewDataset(Dataset ds) throws Exception {
		InsertUpdateBuilder iuBuilder = new InsertUpdateBuilder();
		iuBuilder.addColumnAndValue("timestamp", (ds.getTimestamp() != null ? new java.sql.Timestamp(ds.getTimestamp().getDate().getTime()) : "$CURRENT_TIMESTAMP"));
		iuBuilder.fillWithBusinessClass(ds);
		PreparedStatement ps = getPreparedStatementForInputUpdateBuilder(iuBuilder);
		iuBuilder.fillPreparedStatement(ps);
		executeInsertUpdateAndClose(ps);
		ds.setDatasetID(getLastID());

		if (ds.getAttributes().size() > 0) {
			for (DatasetAttribute attribute : ds.getAttributes()) {
				attribute.setDeviceID(ds.getDeviceID());
				attribute.setDatasetID(ds.getDatasetID());
				insertNewDatasetAttribute(attribute);
			}
		}
		return ds;
	}

	/**
	 * Check if some attribute must to be updated
	 * 
	 * @param dsAttrbiuteToUpdate
	 * @throws Throwable
	 */
	public boolean isDSChanged(String devicedeviceIdentification, Vector<DatasetAttribute> dsAttrbiuteToUpdate) throws Throwable {
		boolean result = false;
		if (devicedeviceIdentification == null || dsAttrbiuteToUpdate == null || dsAttrbiuteToUpdate.size() == 0)
			return false;
		boolean matchHasBeenFound = false;
		Dataset storedDS = getLastDatasetForDeviceIdentification(devicedeviceIdentification);

		if (storedDS == null)
			return true;

		// check the num of the element
		if (dsAttrbiuteToUpdate.size() != storedDS.getAttributes().size())
			return true;

		// the number is equal, so i need to check if there is a chenge on the
		// attributes
		for (DatasetAttribute attrNew : dsAttrbiuteToUpdate) {
			String sha1NewAttr = attrNew.toBsonHash();
			matchHasBeenFound = false;
			for (DatasetAttribute attrStored : storedDS.getAttributes()) {
				String sha1StoredAttr = attrStored.getCheck();
				if (sha1StoredAttr.equals(sha1NewAttr)) {
					matchHasBeenFound = true;
					break;
				}
			}
			if (!matchHasBeenFound) {
				result = true;
				break;
			}
		}
		return result;
	}

	/**
	 * @param dsAttr
	 * @return
	 * @throws Exception
	 */
	public DatasetAttribute insertNewDatasetAttribute(DatasetAttribute dsAttr) throws Exception {
		InsertUpdateBuilder iuBuilder = new InsertUpdateBuilder();
		iuBuilder.addColumnAndValue("timestamp", "$CURRENT_TIMESTAMP");
		iuBuilder.fillWithBusinessClass(dsAttr);
		PreparedStatement ps = getPreparedStatementForInputUpdateBuilder(iuBuilder);
		iuBuilder.fillPreparedStatement(ps);
		executeInsertUpdateAndClose(ps);
		dsAttr.setAttributeID(getLastID());
		return dsAttr;
	}

	/**
	 * @param idDevice
	 * @throws SQLException
	 */
	public void deleteDatasetAttributeForDeviceID(Integer idDevice) throws SQLException {
		DeleteSqlBuilder dSqlBuilder = new DeleteSqlBuilder();
		dSqlBuilder.addTable(DatasetAttribute.class);
		dSqlBuilder.addCondition(true, "id_device = ?");
		PreparedStatement ps = getPreparedStatementForSQLCommand(dSqlBuilder.toString());
		ps.setInt(1, idDevice);
		executeInsertUpdateAndClose(ps);
	}

	/**
	 * @param datasetAttribute
	 * @throws IllegalArgumentException
	 * @throws IllegalAccessException
	 * @throws SQLException
	 */
	public void updateDatasetAttribute(DatasetAttribute datasetAttribute) throws IllegalArgumentException, IllegalAccessException, SQLException {
		InsertUpdateBuilder iuBuilder = new InsertUpdateBuilder(InsertUpdateBuilder.MODE_UPDATE);
		iuBuilder.addColumnAndValue("timestamp", "$CURRENT_TIMESTAMP");
		iuBuilder.fillWithBusinessClass(datasetAttribute);
		iuBuilder.addCondition(true, "id_device = ?");
		iuBuilder.addCondition(true, "id_dataset = ?");
		iuBuilder.addCondition(true, "version = ?");
		iuBuilder.addCondition(true, "id_attribute = ?");
		PreparedStatement ps = getPreparedStatementForInputUpdateBuilder(iuBuilder);
		int idx = iuBuilder.fillPreparedStatement(ps);
		ps.setInt(idx++, datasetAttribute.getDeviceID());
		ps.setInt(idx++, datasetAttribute.getDatasetID());
		ps.setInt(idx++, datasetAttribute.getVersion());
		ps.setInt(idx++, datasetAttribute.getAttributeID());
		executeInsertUpdateAndClose(ps);

	}

	/**
	 * return the dataset attributes for the dataset
	 * 
	 * @param datasetID
	 * @param deviceID
	 * @return
	 * @throws Throwable
	 */
	@SuppressWarnings("unchecked")
	public List<DatasetAttribute> getDatasetsAttributeForDataset(Integer deviceID, Integer datasetID) throws Throwable {
		SqlBuilder dsBuilder = new SqlBuilder();
		SqlTable tds = dsBuilder.addTable(DatasetAttribute.class);
		dsBuilder.addTableColumnToSelect(tds.getTableName(), "*");
		dsBuilder.addCondition(true, "id_device = ?");
		dsBuilder.addCondition(true, "id_dataset = ?");
		PreparedStatement ps = getPreparedStatementForSQLCommand(dsBuilder.toString());
		ps.setInt(1, deviceID);
		ps.setInt(2, datasetID);
		return (List<DatasetAttribute>) executeQueryPreparedStatementAndClose(ps, DatasetAttribute.class, null, null, false);
	}

	/**
	 * @return
	 * @throws Exception
	 */
	public Integer getLastID() throws Exception {
		SqlBuilder sb = new SqlBuilder();
		sb.addTableColumnToSelect("dual", "LAST_INSERT_ID()");
		sb.addTable("dual");
		Integer result = -1;
		ResultSet rs = null;
		PreparedStatement ps = null;
		try {
			ps = getPreparedStatementForSQLCommand(sb.toString());
			rs = ps.executeQuery();
			result = rs.next() ? rs.getInt(1) : -1;
		} catch (Exception e) {
			throw e;
		} finally {
			closeResultSet(rs);
			closePreparedStatement(ps);
		}
		return result;
	}

}
