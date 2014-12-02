/**
 * 
 */
package it.infn.chaos.mds.batchexecution;

import it.infn.chaos.mds.RPCConstants;
import it.infn.chaos.mds.business.UnitServer;
import it.infn.chaos.mds.business.UnitServerCuInstance;

import java.util.Set;

import org.bson.BasicBSONObject;

/**
 * @author bisegni
 * 
 */
public class SystemCommandWorkUnit extends SlowExecutionJob {

	static public class LoadUnloadWorkUnitSetting {
		public UnitServer					unit_server_container	= null;
		public Set<UnitServerCuInstance>	associations			= null;
		public boolean						loadUnload				= true;
	}

	class WorkUnitState {
		UnitServerCuInstance	instance	= null;
		int						state		= 0;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see it.infn.chaos.mds.batchexecution.SlowExecutionJob#executeJob()
	 */
	@Override
	protected void executeJob() throws Throwable {
		LoadUnloadWorkUnitSetting data = (LoadUnloadWorkUnitSetting) getInputData();

		for (UnitServerCuInstance association : data.associations) {
			sendLoadUnloadControlUnitMessage(data.unit_server_container, association, data.loadUnload);
		}
	}

	private void sendLoadUnloadControlUnitMessage(UnitServer unitServer, UnitServerCuInstance instance, boolean load) throws Throwable {
		// send
		if(instance == null)
			return;
		BasicBSONObject msgData = instance.getDriverDescriptionAsBson();
		// msgData.pu
		msgData.append("controlUnitTypeAlias", instance.getCuType());
		msgData.append(RPCConstants.DATASET_DEVICE_ID, instance.getCuId());
		msgData.append("loadControlUnitParam", instance.getCuParam());
		//send message
		Integer reqID = 0;
		sendMessage(unitServer.getIp_port(), "system", load?"loadControlUnit":"unloadControlUnit", msgData);
	}
}
