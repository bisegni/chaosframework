/**
 * 
 */
package it.infn.chaos.mds.rpcaction;

import it.infn.chaos.mds.RPCConstants;
import it.infn.chaos.mds.business.Device;
import it.infn.chaos.mds.da.DeviceDA;
import it.infn.chaos.mds.rpc.server.RPCActionHadler;
import it.infn.chaos.mds.rpc.server.RPCUtils;

import org.bson.BasicBSONObject;
import org.ref.common.exception.RefException;

/**
 * RPC actions for manage the Control Unit registration and device dataset
 * retriving
 * 
 * @author bisegni
 */
public class DeviceQueyHandler extends RPCActionHadler {
	private static final String	SYSTEM				= "system";
	private static final String	GET_LAST_DATASET	= "getCurrentDataset";

	/*
	 * (non-Javadoc)
	 * @see it.infn.chaos.mds.rpc.server.RPCActionHadler#intiHanlder()
	 */
	@Override
	public void intiHanlder() throws RefException {
		addDomainAction(SYSTEM, GET_LAST_DATASET);
	}

	/*
	 * (non-Javadoc)
	 * @see
	 * it.infn.chaos.mds.rpc.server.RPCActionHadler#handleAction(java.lang.String
	 * , java.lang.String, org.bson.BasicBSONObject)
	 */
	@Override
	public BasicBSONObject handleAction(String domain, String action, BasicBSONObject actionData) throws Throwable {
		BasicBSONObject result = null;
		if (domain.equals(SYSTEM)) {
			if (action.equals(GET_LAST_DATASET)) {
				result = getLastDataset(actionData);
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
	private BasicBSONObject getLastDataset(BasicBSONObject actionData) throws Throwable {
		DeviceDA dDA = null;
		BasicBSONObject result = null;
		try {
			String deviceIdentification = actionData.getString(RPCConstants.DATASET_DEVICE_ID);
			dDA = getDataAccessInstance(DeviceDA.class);
			Device d = dDA.getDeviceFromDeviceIdentification(deviceIdentification);
			result = new BasicBSONObject();
			if (d.getCuInstance() != null)
				result.append(RPCConstants.CONTROL_UNIT_INSTANCE, d.getCuInstance());
			if (d.getNetAddress() != null)
				result.append(RPCConstants.CONTROL_UNIT_INSTANCE_NETWORK_ADDRESS, d.getNetAddress());
			result.append(RPCConstants.DATASET_DESCRIPTION, d.toBson());
		} catch (Throwable e) {
			RPCUtils.addRefExceptionElementToBson(result, "CURegistration::getLastDataset", e.getMessage(), 0);
			throw e;
		} finally {
			dDA.getConnection().close();
		}
		return result;
	}

}
