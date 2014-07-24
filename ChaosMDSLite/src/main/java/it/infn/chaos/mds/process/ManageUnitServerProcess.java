package it.infn.chaos.mds.process;

import it.infn.chaos.mds.business.UnitServer;
import it.infn.chaos.mds.da.UnitServerDA;

import java.util.List;
import java.util.Vector;

import org.ref.server.interapplicationenvironment.ProcessActionDescription;
import org.ref.server.interapplicationenvironment.ProcessDescription;
import org.ref.server.process.RefProcess;

@ProcessDescription
public class ManageUnitServerProcess extends RefProcess {

	/**
	 * 
	 */
	private static final long	serialVersionUID	= -8527016113254216852L;

	/**
	 * return all unit server
	 * 
	 * @return
	 * @throws Throwable
	 */
	@ProcessActionDescription
	public List<UnitServer> getAllUnitServer() throws Throwable {
		Vector<UnitServer> usList = new Vector<UnitServer>();
		UnitServerDA usDA = (UnitServerDA) getDataAccessInstance(UnitServerDA.class);
		usList = usDA.getAlUnitServer();
		return (List<UnitServer>) usList;
	}

	/**
	 * 
	 * @param unitServerAlias
	 * @return
	 * @throws Throwable
	 */
	@ProcessActionDescription
	public UnitServer getUnitServerByIdentification(String unitServerAlias) throws Throwable {
		UnitServerDA usDA = (UnitServerDA) getDataAccessInstance(UnitServerDA.class);
		return usDA.getUnitServerByAlias(unitServerAlias);
	}
}
