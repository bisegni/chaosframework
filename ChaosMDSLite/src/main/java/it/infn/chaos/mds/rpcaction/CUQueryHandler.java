/**
 *
 */
package it.infn.chaos.mds.rpcaction;

import it.infn.chaos.mds.RPCConstants;
import it.infn.chaos.mds.SingletonServices;
import it.infn.chaos.mds.batchexecution.UnitServerACK;
import it.infn.chaos.mds.batchexecution.WorkUnitACK;
import it.infn.chaos.mds.business.Device;
import it.infn.chaos.mds.business.UnitServer;
import it.infn.chaos.mds.da.DeviceDA;
import it.infn.chaos.mds.da.UnitServerDA;
import it.infn.chaos.mds.rpc.server.RPCActionHadler;

import java.sql.SQLException;
import java.util.ListIterator;

import org.bson.BasicBSONObject;
import org.bson.types.BasicBSONList;
import org.ref.common.exception.RefException;
import org.ref.common.helper.ExceptionHelper;

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
		addDomainAction(SYSTEM, CUQueryHandler.REGISTER_CONTROL_UNIT);
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
			} else if (action.equals(RPCConstants.MDS_REGISTER_UNIT_SERVER)) {
				result = registerUnitServer(actionData);
			}
		}
		return result;
	}

	/**
	 * Register an unit server
	 * 
	 * @param actionData
	 * @return
	 * @throws SQLException
	 * @throws IllegalAccessException
	 * @throws InstantiationException
	 */
	private BasicBSONObject registerUnitServer(BasicBSONObject actionData) throws RefException {
		UnitServerDA usDA = null;
		try {
			usDA = getDataAccessInstance(UnitServerDA.class);
			if (actionData == null)
				new RefException("No info forwarded for unit server registration", 1, "CUQueryHandler::registerUnitServer");
			UnitServer us = new UnitServer();
			us.setAlias(actionData.getString(RPCConstants.MDS_REGISTER_UNIT_SERVER_ALIAS));
			us.setIp_port(actionData.getString(RPCConstants.CONTROL_UNIT_INSTANCE_NETWORK_ADDRESS));
			BasicBSONList bsonAttributesArray = (BasicBSONList) actionData.get(RPCConstants.MDS_REGISTER_UNIT_SERVER_CONTROL_UNIT_ALIAS);
			ListIterator<Object> dsDescIter = bsonAttributesArray.listIterator();
			while (dsDescIter.hasNext()) {
				us.addPublischedCU(dsDescIter.next().toString());
			}

			if (usDA.unitServerAlreadyRegistered(us)) {
				usDA.updateUnitServer(us);
			} else {
				usDA.insertNewUnitServer(us);
			}
			actionData.append(RPCConstants.MDS_REGISTER_UNIT_SERVER_RESULT, (int) 5);
			closeDataAccess(usDA, true);
		} catch (InstantiationException e) {
			actionData.append(RPCConstants.MDS_REGISTER_UNIT_SERVER_RESULT, (int) 6);
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 0, "CUQueryHandler::registerUnitServer");
		} catch (IllegalAccessException e) {
			actionData.append(RPCConstants.MDS_REGISTER_UNIT_SERVER_RESULT, (int) 6);
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 1, "CUQueryHandler::registerUnitServer");
		} catch (SQLException e) {
			actionData.append(RPCConstants.MDS_REGISTER_UNIT_SERVER_RESULT, (int) 6);
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 2, "CUQueryHandler::registerUnitServer");
		} finally {
			try {
				closeDataAccess(usDA, false);
			} catch (SQLException e) {
				// e.printStackTrace();
			}
			try {
				SingletonServices.getInstance().getSlowExecution().submitJob(UnitServerACK.class.getName(), actionData);
			} catch (InstantiationException e) {
				e.printStackTrace();
			} catch (IllegalAccessException e) {
				e.printStackTrace();
			}
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
		UnitServerDA usDA = null;
		BasicBSONObject ackPack = new BasicBSONObject();
		boolean sendACK = false;
		try {
			dDA = getDataAccessInstance(DeviceDA.class);
			usDA = getDataAccessInstance(UnitServerDA.class);

			if (actionData == null)
				return null;
			// check for CU presence
			String controlUnitNetAddress = actionData.containsField(RPCConstants.CONTROL_UNIT_INSTANCE_NETWORK_ADDRESS) ? actionData.getString(RPCConstants.CONTROL_UNIT_INSTANCE_NETWORK_ADDRESS) : null;
			if (controlUnitNetAddress == null)
				throw new RefException("No network address found", 0, "DeviceDA::controlUnitValidationAndRegistration");
			String controlUnitInstance = actionData.containsField(RPCConstants.CONTROL_UNIT_INSTANCE) ? actionData.getString(RPCConstants.CONTROL_UNIT_INSTANCE) : null;
			if (controlUnitInstance == null)
				throw new RefException("No control unit instance found", 1, "DeviceDA::controlUnitValidationAndRegistration");
			if (!actionData.containsField("mds_control_key")) {
				throw new RefException("No self_managed_work_unit key found", 1, "DeviceDA::controlUnitValidationAndRegistration");
			}

			String control_key = actionData.getString("mds_control_key");

			d = new Device();
			d.setCuInstance(controlUnitInstance);
			d.setNetAddress(controlUnitNetAddress);
			d.fillFromBson(actionData);
			// add device id into ack pack
			ackPack.append(RPCConstants.CONTROL_UNIT_INSTANCE_NETWORK_ADDRESS, actionData.getString(RPCConstants.CONTROL_UNIT_INSTANCE_NETWORK_ADDRESS));
			ackPack.append(RPCConstants.DATASET_DEVICE_ID, actionData.getString(RPCConstants.DATASET_DEVICE_ID));
			sendACK = true;
			if (usDA.cuIDIsMDSManaged(actionData.getString(RPCConstants.DATASET_DEVICE_ID)) && !control_key.equals("mds")) {
				ackPack.append(RPCConstants.MDS_REGISTER_UNIT_SERVER_RESULT, (int) 9);
				return null;
			}
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
				usDA.setState(d.getDeviceIdentification(), "Registered");
			}
			
			closeDataAccess(dDA, true);
			closeDataAccess(usDA, true);
			ackPack.append(RPCConstants.MDS_REGISTER_UNIT_SERVER_RESULT, (int) 5);
		} catch (RefException e) {
			actionData.append(RPCConstants.MDS_REGISTER_UNIT_SERVER_RESULT, (int) 6);
			try {
				closeDataAccess(dDA, false);
			} catch (SQLException e1) {
			}
			try {
				closeDataAccess(usDA, false);
			} catch (SQLException e1) {
			}
			throw new RefException(e.getMessage(), 3, "CUQueryHandler::registerControUnit");
		} catch (Throwable e) {
			ackPack.append(RPCConstants.MDS_REGISTER_UNIT_SERVER_RESULT, (int) 5);
			try {
				closeDataAccess(dDA, false);
			} catch (SQLException e1) {
			}
			try {
				closeDataAccess(usDA, false);
			} catch (SQLException e1) {
			}
			throw new RefException(e.getMessage(), 4, "CUQueryHandler::registerControUnit");
		} finally {
			try {
				if (sendACK == true)
					SingletonServices.getInstance().getSlowExecution().submitJob(WorkUnitACK.class.getName(), ackPack);
			} catch (InstantiationException e) {
				e.printStackTrace();
			} catch (IllegalAccessException e) {
				e.printStackTrace();
			}
		}
		return result;
	}
}
