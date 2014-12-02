/**
 * 
 */
package it.infn.chaos.mds.batchexecution;

import java.util.Iterator;
import java.util.List;

import org.bson.BasicBSONObject;
import org.bson.types.BasicBSONList;

import it.infn.chaos.mds.RPCConstants;
import it.infn.chaos.mds.business.DataServer;
import it.infn.chaos.mds.business.Device;
import it.infn.chaos.mds.rpcaction.DeviceDescriptionUtility;



/**
 * @author Andrea Michelotti
 *
 */
public class InitCU extends SlowExecutionJob {

	/* (non-Javadoc)
	 * @see it.infn.chaos.mds.slowexecution.SlowExecution.SlowExecutioJob#executeJob()
	 */
	protected void executeJob() throws Throwable {
		Object[] initData = (Object[]) getInputData();
		Device device = (Device)initData[0];
		List<DataServer> cdsAddress = (List<DataServer>)initData[1];
		
		BasicBSONObject result = (BasicBSONObject) device.toBson();
		result.append("cu_uuid", device.getCuInstance());
		
		// add live server
		BasicBSONList liveServers = new BasicBSONList();
		for (Iterator<DataServer> iterator = cdsAddress.iterator(); iterator.hasNext();) {
			DataServer dataServer = (DataServer) iterator.next();
			liveServers.add(String.format(dataServer.getHostname()));
		}
		if (liveServers.size() > 0)
			result.append(RPCConstants.LIVE_DATASERVER_HOST_PORT, liveServers);
		result.append(RPCConstants.DEVICE_SCHEDULE_DELAY, 1000000);
		sendMessage(device.getNetAddress(), device.getCuInstance(), "initControlUnit", result);
	}

}
