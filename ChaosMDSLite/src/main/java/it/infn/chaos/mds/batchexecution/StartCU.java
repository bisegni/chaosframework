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
public class StartCU extends SlowExecutionJob {

	/* (non-Javadoc)
	 * @see it.infn.chaos.mds.slowexecution.SlowExecution.SlowExecutioJob#executeJob()
	 */
	protected void executeJob() throws Throwable {
		String[] data = (String[]) getInputData();
		sendMessage(data[0], data[1], "startControlUnit", null);
	}

}
