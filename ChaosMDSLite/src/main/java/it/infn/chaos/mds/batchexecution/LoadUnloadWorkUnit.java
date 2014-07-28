/**
 * 
 */
package it.infn.chaos.mds.batchexecution;

import it.infn.chaos.mds.business.UnitServer;
import it.infn.chaos.mds.business.UnitServerCuInstance;

import java.util.Set;

import org.bson.BasicBSONObject;

/**
 * @author bisegni
 * 
 */
public class LoadUnloadWorkUnit extends SlowExecutionJob {

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

	}

	private void sendLoadUnloadControlUnitMessage(UnitServerCuInstance instance) {
		// send
		BasicBSONObject msgData = new BasicBSONObject();
		// msgData.pu
	}
}
