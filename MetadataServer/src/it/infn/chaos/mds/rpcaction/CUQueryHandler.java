/**
 * 
 */
package it.infn.chaos.mds.rpcaction;

import it.infn.chaos.mds.RPCConstants;
import it.infn.chaos.mds.business.Device;
import it.infn.chaos.mds.da.DataServerDA;
import it.infn.chaos.mds.da.DeviceDA;
import it.infn.chaos.mds.rpc.server.RPCActionHadler;

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
		addDomainAction(SYSTEM, REGISTER_CONTROL_UNIT);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see it.infn.chaos.mds.rpc.server.RPCActionHadler#handleAction(java.lang.String ,
	 * java.lang.String, org.bson.BasicBSONObject)
	 */
	@Override
	public BasicBSONObject handleAction(String domain, String action, BasicBSONObject actionData) throws Throwable {
		BasicBSONObject result = null;
		if (domain.equals(SYSTEM)) {
			if (action.equals(REGISTER_CONTROL_UNIT)) {
				result = registerControUnit(actionData);
			} else if (action.equals(HEARTBEAT_CONTROL_UNIT)) {
				result = heartbeatControUnit(actionData);
			}
		}
		return result;
	}

	/**
	 * Execute l'heartbeat of the control unit
	 * 
	 * @param actionData
	 * @return
	 */
	private BasicBSONObject heartbeatControUnit(BasicBSONObject actionData) {
		BasicBSONObject result = null;
		return result;
	}

	/**
	 * @return
	 * @throws Throwable
	 */
	private BasicBSONObject registerControUnit(BasicBSONObject actionData) throws Throwable {
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
				throw new RefException("No control unit instance found", 1, "DeviceDA::controlUnitValidationAndRegistration");
			String controlUnitInstance = actionData.containsField(RPCConstants.CONTROL_UNIT_INSTANCE) ? actionData.getString(RPCConstants.CONTROL_UNIT_INSTANCE) : null;
			if (controlUnitInstance == null)
				throw new RefException("No control unit instance found", 1, "DeviceDA::controlUnitValidationAndRegistration");
			// get the device list for the single CU
			BasicBSONList dsDesc = (BasicBSONList) (actionData.containsField(RPCConstants.DATASET_DESCRIPTION) ? actionData.get(RPCConstants.DATASET_DESCRIPTION) : null);
			if (dsDesc == null)
				throw new RefException("No device is defined in control unit", 2, "DeviceDA::controlUnitValidationAndRegistration");
			ListIterator<Object> devicesDS = dsDesc.listIterator();
			while (devicesDS.hasNext()) {
				BasicBSONObject devDesc = (BasicBSONObject) devicesDS.next();
				d = new Device();
				d.setCuInstance(controlUnitInstance);
				d.setNetAddress(controlUnitNetAddress);
				d.fillFromBson(devDesc);

				// check for deviceID presence
				if (dDA.isDeviceIDPresent(d.getDeviceIdentification())) {
					// the device is already present i need to check for dataset
					// change

					if (dDA.isDSChanged(d.getDeviceIdentification(), d.getDataset().getAttributes())) {
						Integer deviceID = dDA.getDeviceIdFormInstance(d.getDeviceIdentification());
						// Dataset newDatasetToInsertForDevice =
						// dDA.getLastDatasetForDeviceInstance(d.getDeviceInstance());
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
						result = DeviceDescriptionUtility.composeStartupCommandForDeviceIdentification(d.getDeviceIdentification(),dDA , getDataAccessInstance(DataServerDA.class));
						
						//set the destination for the message
						result.append(RPCConstants.CS_CMDM_REMOTE_HOST_IP, d.getNetAddress());
						result.append(RPCConstants.CS_CMDM_ACTION_DOMAIN, "system");
						result.append(RPCConstants.CS_CMDM_ACTION_NAME, "initControlUnit");
					}
				}
			}

			closeDataAccess(dDA, true);
		} catch (RefException e) {
			closeDataAccess(dDA, false);
			throw e;
		} catch (Throwable e) {
			closeDataAccess(dDA, false);
			throw e;
		}
		return result;
	}
}
