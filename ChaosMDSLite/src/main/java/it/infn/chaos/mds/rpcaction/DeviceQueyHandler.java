/**
 * 
 */
package it.infn.chaos.mds.rpcaction;

import java.sql.SQLException;
import java.util.List;

import it.infn.chaos.mds.RPCConstants;
import it.infn.chaos.mds.business.Device;
import it.infn.chaos.mds.da.DataServerDA;
import it.infn.chaos.mds.da.DeviceDA;
import it.infn.chaos.mds.rpc.server.RPCActionHadler;
import it.infn.chaos.mds.rpc.server.RPCUtils;

import org.bson.BasicBSONObject;
import org.bson.types.BasicBSONList;
import org.ref.common.exception.RefException;

/**
 * RPC actions for manage the Control Unit registration and device dataset retriving
 * 
 * @author bisegni
 */
public class DeviceQueyHandler extends RPCActionHadler {
	private static final String	SYSTEM					= "system";
	private static final String	GET_LAST_DATASET		= "getCurrentDataset";
	private static final String	GET_ALL_ACTIVE_DEVICE	= "getAllActiveDevice";
	private static final String	GET_DEVICE_NODE_NETWORK	= "getNodeNetworkAddress";

	/*
	 * (non-Javadoc)
	 * 
	 * @see it.infn.chaos.mds.rpc.server.RPCActionHadler#intiHanlder()
	 */
	@Override
	public void intiHanlder() throws RefException {
		addDomainAction(SYSTEM, GET_LAST_DATASET);
		addDomainAction(SYSTEM, GET_ALL_ACTIVE_DEVICE);
		addDomainAction(SYSTEM, GET_DEVICE_NODE_NETWORK);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see it.infn.chaos.mds.rpc.server.RPCActionHadler#handleAction(java.lang.String ,
	 * java.lang.String, org.bson.BasicBSONObject)
	 */
	@Override
	public BasicBSONObject handleAction(String domain, String action, BasicBSONObject actionData) throws RefException {
		BasicBSONObject result = null;
		if (domain.equals(SYSTEM)) {
			if (action.equals(GET_LAST_DATASET)) {
				result = getDeviceDataseFromDeviceID(actionData);
			} else if (action.equals(GET_ALL_ACTIVE_DEVICE)) {
				result = getAllActiveDevice(actionData);
			}else if (action.equals(GET_DEVICE_NODE_NETWORK)) {
				result = getNodeNetworkAddress(actionData);
			}
		}
		return result;
	}

	/**
	 * 
	 * @param actionData
	 * @return
	 * @throws Throwable
	 */
	private BasicBSONObject getNodeNetworkAddress(BasicBSONObject actionData) throws RefException {
		DeviceDA dDA = null;
		BasicBSONObject result = null;
		try {
			dDA = getDataAccessInstance(DeviceDA.class);
			String deviceIdentification = actionData.getString(RPCConstants.DATASET_DEVICE_ID);
			Device d = dDA.getDeviceFromDeviceIdentification(deviceIdentification);
			result = new BasicBSONObject();
			result.append(RPCConstants.CONTROL_UNIT_INSTANCE_NETWORK_ADDRESS, d.getNetAddress());
			result.append(RPCConstants.CONTROL_UNIT_INSTANCE, d.getCuInstance());
		} catch (Throwable e) {
			RPCUtils.addRefExceptionElementToBson(result, "DeviceQueyHandler::getDeviceDataseFromDeviceID", e.getMessage(), 0);
			throw new RefException("getNodeNetworkAddress error", 1, "CUQueryHandler::getNodeNetworkAddress");
		} finally {
			try {
				closeDataAccess(dDA, false);
			} catch (SQLException e) {
			}
		}
		return result;
	}

	/**
	 * 
	 * @param actionData
	 * @return
	 * @throws Throwable
	 */
	private BasicBSONObject getAllActiveDevice(BasicBSONObject actionData) throws RefException {
		DeviceDA dDA = null;
		BasicBSONObject result = new BasicBSONObject();
		try {

			dDA = getDataAccessInstance(DeviceDA.class);
			List<Device> devices = dDA.getAllActiveDevice();
			BasicBSONList deviceArray = new BasicBSONList();
			for (Device d : devices) {
				deviceArray.add(d.getDeviceIdentification());
			}
			result.append(RPCConstants.DATASET_DEVICE_ID, deviceArray);
		} catch (Throwable e) {
			throw new RefException("getNodeNetworkAddress error", 1, "CUQueryHandler::getNodeNetworkAddress");
		} finally {
			try {
				closeDataAccess(dDA, false);
			} catch (SQLException e) {
			}
		}
		return result;
	}

	/**
	 * Return the last dataset for an device id
	 * 
	 * @param actionData
	 * @return
	 * @throws Throwable
	 */
	private BasicBSONObject getDeviceDataseFromDeviceID(BasicBSONObject actionData) throws RefException {
		DeviceDA dDA = null;
		DataServerDA dsDA = null;
		BasicBSONObject result = null;
		try {
			System.out.println("getDeviceDataseFromDeviceID");
			dDA = getDataAccessInstance(DeviceDA.class);
			dsDA = getDataAccessInstance(DataServerDA.class);
			String deviceIdentification = actionData.getString(RPCConstants.DATASET_DEVICE_ID);
			result = DeviceDescriptionUtility.composeStartupCommandForDeviceIdentification(deviceIdentification, dDA, dsDA, false);
		} catch (Throwable e) {
			throw new RefException("getDeviceDataseFromDeviceID error", 1, "CUQueryHandler::getDeviceDataseFromDeviceID");
		} finally {
			if (dDA != null)
				try {
					dDA.getConnection().close();
				} catch (SQLException e) {
					e.printStackTrace();
				}
			if (dsDA != null)
				try {
					dsDA.getConnection().close();
				} catch (SQLException e) {
					e.printStackTrace();
				}
		}
		return result;
	}
}
