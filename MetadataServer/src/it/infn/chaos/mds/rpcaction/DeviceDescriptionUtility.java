/**
 * 
 */
package it.infn.chaos.mds.rpcaction;

import it.infn.chaos.mds.RPCConstants;
import it.infn.chaos.mds.business.DataServer;
import it.infn.chaos.mds.business.Device;
import it.infn.chaos.mds.da.DataServerDA;
import it.infn.chaos.mds.da.DeviceDA;

import java.util.Iterator;
import java.util.List;

import org.bson.BasicBSONObject;
import org.bson.types.BasicBSONList;

/**
 * @author bisegni
 *
 */
public class DeviceDescriptionUtility {

	/**
	 * Compose the BSON object for startup initialization of device
	 * 
	 * @param deviceIdentification
	 * @return
	 * @throws Throwable
	 */
	static public BasicBSONObject composeStartupCommandForDeviceIdentification(String deviceIdentification, DeviceDA dDA, DataServerDA dsDA) throws Throwable {
		BasicBSONObject result = null;
		
		Device d = dDA.getDeviceFromDeviceIdentification(deviceIdentification);
		result = (BasicBSONObject) d.toBson();
		result.append("cu_uuid", d.getCuInstance());
		result.append(RPCConstants.CONTROL_UNIT_AUTOSTART, 1);
		//add live server 
		BasicBSONList liveServers = new BasicBSONList();
		List<DataServer> list = dsDA.getAllDataServer(true);
		for (Iterator<DataServer> iterator = list.iterator(); iterator.hasNext();) {
			DataServer dataServer = (DataServer) iterator.next();
			liveServers.add(String.format("%s:%d", dataServer.getHostname(), dataServer.getPort()));
		}
		if(liveServers.size()>0)result.append(RPCConstants.LIVE_DATASERVER_HOST_PORT, liveServers);
		result.append(RPCConstants.DEVICE_SCHEDULE_DELAY, 1000000);
		
		return result;
	}
}
