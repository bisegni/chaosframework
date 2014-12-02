/**
 * 
 */
package it.infn.chaos.mds.batchexecution;

import org.bson.BasicBSONObject;

import it.infn.chaos.mds.RPCConstants;



/**
 * @author Andrea Michelotti
 *
 */
public class StopCU extends SlowExecutionJob {

	/* (non-Javadoc)
	 * @see it.infn.chaos.mds.slowexecution.SlowExecution.SlowExecutioJob#executeJob()
	 */
	protected void executeJob() throws Throwable {
		BasicBSONObject data = (BasicBSONObject) getInputData();
		sendMessage(data.getString(RPCConstants.CONTROL_UNIT_INSTANCE_NETWORK_ADDRESS), "system", "stopControlUnit", data);
	}

}
