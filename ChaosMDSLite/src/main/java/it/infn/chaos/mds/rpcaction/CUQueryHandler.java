/**
 *
 */
package it.infn.chaos.mds.rpcaction;

import it.infn.chaos.mds.RPCConstants;
import it.infn.chaos.mds.business.DatasetAttribute;
import it.infn.chaos.mds.business.Device;
import it.infn.chaos.mds.da.DataServerDA;
import it.infn.chaos.mds.da.DeviceDA;
import it.infn.chaos.mds.rpc.server.RPCActionHadler;

import java.sql.SQLException;
import java.util.ListIterator;

import org.bson.BasicBSONObject;
import org.bson.types.BasicBSONList;
import org.ref.common.exception.RefException;

/**
 * RPC actions for manage the Control Unit registration and device dataset retriving
 * 
 * @author bisegni
 */
public class CUQueryHandler extends RPCActionHadler {
	private static final String	SYSTEM					= "system";
	private static final String	REGISTER_CONTROL_UNIT	= "registerControlUnit";
	private static final String	HEARTBEAT_CONTROL_UNIT	= "heartbeatControlUnit";

	/*
	 * (non-Javadoc)
	 * 
	 * @see it.infn.chaos.mds.rpc.server.RPCActionHadler#intiHanlder()
	 */
	@Override
	public void intiHanlder() throws RefException {
		addDomainAction(SYSTEM, RPCConstants.MDS_REGISTER_UNIT_SERVER);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see it.infn.chaos.mds.rpc.server.RPCActionHadler#handleAction(java.lang.String , java.lang.String, org.bson.BasicBSONObject)
	 */
	@Override
	public BasicBSONObject handleAction(String domain, String action, BasicBSONObject actionData) throws RefException {
		BasicBSONObject result = null;
		if (domain.equals(SYSTEM)) {
			if (action.equals(REGISTER_CONTROL_UNIT)) {
				result = registerControUnit(actionData);
			} else if (action.equals(HEARTBEAT_CONTROL_UNIT)) {
				result = heartbeat(actionData);
			}else if (action.equals(RPCConstants.MDS_REGISTER_UNIT_SERVER)) {
				result = registerUnitServer(actionData);
			}
		}
		return result;
	}

	/**
	 * Register an unit server
	 * @param actionData
	 * @return
	 */
	private BasicBSONObject registerUnitServer(BasicBSONObject actionData) {
		if(actionData == null) new RefException("No info forwarded for unit server registration", 1, "CUQueryHandler::registerUnitServer");
		String unitName = actionData.getString(RPCConstants.MDS_REGISTER_UNIT_SERVER_ALIAS);
		
		BasicBSONList bsonAttributesArray = (BasicBSONList) actionData.get(RPCConstants.MDS_REGISTER_UNIT_SERVER_CONTROL_UNIT_ALIAS);
		ListIterator<Object> dsDescIter = bsonAttributesArray.listIterator();
		while (dsDescIter.hasNext()) {
			System.out.println(dsDescIter.next().toString());
		}
		return null;
	}

	/**
	 * Execute l'heartbeat of the control unit
	 * 
	 * @param actionData
	 * @return
	 */
	private BasicBSONObject heartbeat(BasicBSONObject actionData) throws RefException {
		BasicBSONObject result = null;
		if (!actionData.containsField(RPCConstants.DATASET_DEVICE_ID))
			throw new RefException("Message desn't contain deviceid");
		DeviceDA dDA = null;
		try {
			dDA = getDataAccessInstance(DeviceDA.class);
			dDA.performDeviceHB(actionData.getString(RPCConstants.DATASET_DEVICE_ID));
		} catch (Exception e) {
			throw new RefException("heartbeat error", 1, "CUQueryHandler::heartbeat");
		} catch (Throwable e) {
			throw new RefException("heartbeat error", 1, "CUQueryHandler::heartbeat");
		} finally {
			try {
				closeDataAccess(dDA, false);
			} catch (SQLException e) {
				e.printStackTrace();
			}
		}

		return result;
	}

	/**
	 * @return
	 * @throws Throwable
	 */
	private BasicBSONObject registerControUnit(BasicBSONObject actionData) throws RefException {
		Device d = null;
		BasicBSONObject result = null;
		DeviceDA dDA = null;
		try {
			dDA = getDataAccessInstance(DeviceDA.class);
			if (actionData == null)
				return null;
			// check for CU presence
			String controlUnitNetAddress = actionData.containsField(RPCConstants.CONTROL_UNIT_INSTANCE_NETWORK_ADDRESS) ? actionData.getString(RPCConstants.CONTROL_UNIT_INSTANCE_NETWORK_ADDRESS) : null;
			if (controlUnitNetAddress == null)
				throw new RefException("No network address found", 0, "DeviceDA::controlUnitValidationAndRegistration");
			String controlUnitInstance = actionData.containsField(RPCConstants.CONTROL_UNIT_INSTANCE) ? actionData.getString(RPCConstants.CONTROL_UNIT_INSTANCE) : null;
			if (controlUnitInstance == null)
				throw new RefException("No control unit instance found", 1, "DeviceDA::controlUnitValidationAndRegistration");

			d = new Device();
			d.setCuInstance(controlUnitInstance);
			d.setNetAddress(controlUnitNetAddress);
			d.fillFromBson(actionData);

			// check for deviceID presence
			if (dDA.isDeviceIDPresent(d.getDeviceIdentification())) {
				// the device is already present i need to check for dataset
				// change

				if (dDA.isDSChanged(d.getDeviceIdentification(), d.getDataset().getAttributes())) {
					Integer deviceID = dDA.getDeviceIdFormInstance(d.getDeviceIdentification());
					d.getDataset().setDeviceID(deviceID);
					// add new dataset
					dDA.insertNewDataset(d.getDataset());
				}
				// update the CU id for this device, it can be changed
				dDA.updateCUInstanceAndAddressForDeviceID(d.getDeviceIdentification(), d.getCuInstance(), d.getNetAddress());
			} else {
				dDA.insertDevice(d);
			}

			if (d != null) {
				dDA.performDeviceHB(d.getDeviceIdentification());

				// at this point i need to check if thedevice need to be initialized
				if (dDA.isDeviceToBeInitialized(d.getDeviceIdentification())) {
					// send rpc command to initialize the device
					result = DeviceDescriptionUtility.composeStartupCommandForDeviceIdentification(d.getDeviceIdentification(), dDA, getDataAccessInstance(DataServerDA.class), true);
					result.append(RPCConstants.CS_CMDM_REMOTE_HOST_IP, d.getNetAddress());
					result.append(RPCConstants.CS_CMDM_ACTION_DOMAIN, "system");
					result.append(RPCConstants.CS_CMDM_ACTION_NAME, "initControlUnit");
				}
			}
			// }

			closeDataAccess(dDA, true);
		} catch (RefException e) {
			try {
				closeDataAccess(dDA, false);
			} catch (SQLException e1) {
			}
			throw new RefException(e.getMessage(), 3, "CUQueryHandler::registerControUnit");
		} catch (Throwable e) {
			try {
				closeDataAccess(dDA, false);
			} catch (SQLException e1) {
			}
			throw new RefException(e.getMessage(), 4, "CUQueryHandler::registerControUnit");
		}
		return result;
	}
}
