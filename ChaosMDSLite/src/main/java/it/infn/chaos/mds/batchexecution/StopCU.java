/**
 * 
 */
package it.infn.chaos.mds.batchexecution;

import it.infn.chaos.mds.business.Device;



/**
 * @author Andrea Michelotti
 *
 */
public class StopCU extends SlowExecutionJob {

	/* (non-Javadoc)
	 * @see it.infn.chaos.mds.slowexecution.SlowExecution.SlowExecutioJob#executeJob()
	 */
	protected void executeJob() throws Throwable {
		Device device = (Device) getInputData();
		sendMessage(device.getNetAddress(), device.getCuInstance(), "stopControlUnit", null);
	}

}
