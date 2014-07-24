/**
 * 
 */
package it.infn.chaos.mds.process;

import it.infn.chaos.mds.business.DataServer;
import it.infn.chaos.mds.da.DataServerDA;

import java.util.List;

import org.ref.server.interapplicationenvironment.ProcessDescription;
import org.ref.server.process.RefProcess;

/**
 * @author bisegni
 * 
 */
@ProcessDescription
@SuppressWarnings("serial")
public class ManageServerProcess extends RefProcess {

	/**
	 * 
	 * @throws Throwable
	 */
	public void addNewServer() throws Throwable {
		try {
			DataServerDA dsDA = (DataServerDA) getDataAccessInstance(DataServerDA.class);
			DataServer newDataServer = new DataServer();
			newDataServer.setHostname("server decription");
			newDataServer.setIsLiveServer(false);
			dsDA.saveServer(newDataServer);
			commit();
		} catch (Exception e) {
			rollback();
			throw e;
		}

	}

	/**
	 * 
	 * @param ds
	 * @throws Throwable
	 */
	public void updateDataServer(DataServer ds) throws Throwable {
		try {
			DataServerDA dsDA = (DataServerDA) getDataAccessInstance(DataServerDA.class);
			dsDA.updateServer(ds);
			commit();
		} catch (Exception e) {
			rollback();
			throw e;
		}

	}

	/**
	 * 
	 * @return
	 * @throws Throwable
	 */
	public List<DataServer> getAllServer() throws Throwable {
		DataServerDA dsDA = (DataServerDA) getDataAccessInstance(DataServerDA.class);
		return dsDA.getAllDataServer();
	}

	/**
	 * remove a dataserver
	 * @param dsToUpdate
	 * @throws Exception 
	 */
	public void deleteServer(DataServer dsToUpdate) throws Exception {
		try {
			DataServerDA dsDA = (DataServerDA) getDataAccessInstance(DataServerDA.class);
			dsDA.deleteDataserver(dsToUpdate.getIdServer());
			commit();
		} catch (Exception e) {
			rollback();
			throw e;
		}
		
	}
}
