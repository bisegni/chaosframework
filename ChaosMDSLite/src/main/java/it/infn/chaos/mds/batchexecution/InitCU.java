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
public class InitCU extends SlowExecutionJob {

	/* (non-Javadoc)
	 * @see it.infn.chaos.mds.slowexecution.SlowExecution.SlowExecutioJob#executeJob()
	 */
	protected void executeJob() throws Throwable {
		String[] cu = (String[])getInputData();
		sendMessage(cu[0], cu[1], "initControlUnit", null);
	}

}
