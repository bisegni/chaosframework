package it.infn.chaos.mds.process;

import it.infn.chaos.mds.RPCConstants;
import it.infn.chaos.mds.business.DatasetAttribute;
import it.infn.chaos.mds.business.Device;
import it.infn.chaos.mds.business.UnitServer;
import it.infn.chaos.mds.business.UnitServerCuInstance;
import it.infn.chaos.mds.business.DeviceClass;
import it.infn.chaos.mds.da.DeviceDA;
import it.infn.chaos.mds.da.UnitServerDA;

import java.sql.SQLException;
import java.util.Collection;
import java.util.List;
import java.util.Set;
import java.util.UUID;
import java.util.Vector;

import org.bson.BasicBSONObject;
import org.ref.common.exception.RefException;
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
	 * Create new unit server with unique alias
	 * 
	 * @throws ClassNotFoundException
	 * @throws IllegalAccessException
	 * @throws InstantiationException
	 * @throws RefException
	 * @throws SQLException
	 */
	public void createNewUnitServer() throws InstantiationException, IllegalAccessException, ClassNotFoundException, RefException, SQLException {
		UnitServerDA usDA = (UnitServerDA) getDataAccessInstance(UnitServerDA.class);
		UnitServer us = new UnitServer();
		us.setAlias(UUID.randomUUID().toString().substring(0, 8));
		usDA.insertNewUnitServer(us);
		commit();
	}

	/**
	 * 
	 * @param eventData
	 * @throws ClassNotFoundException
	 * @throws IllegalAccessException
	 * @throws InstantiationException
	 * @throws RefException
	 * @throws SQLException
	 */
	public void updaUnitServerProperty(UnitServer eventData) throws InstantiationException, IllegalAccessException, ClassNotFoundException, RefException, SQLException {
		UnitServerDA usDA = (UnitServerDA) getDataAccessInstance(UnitServerDA.class);
		usDA.updateUnitServerProperty(eventData);
		commit();
	}


	public void deleteSecurityKeys(UnitServer eventData) throws InstantiationException, IllegalAccessException, ClassNotFoundException, RefException, SQLException {
		UnitServerDA usDA = (UnitServerDA) getDataAccessInstance(UnitServerDA.class);
		usDA.deleteSecurityKeys(eventData);
		commit();
	}
	
	/**
	 * 
	 * @param unitServerSelected
	 * @throws InstantiationException
	 * @throws IllegalAccessException
	 * @throws ClassNotFoundException
	 * @throws SQLException
	 * @throws RefException
	 */
	public void deleteUnitServer(String unitServerSelected) throws InstantiationException, IllegalAccessException, ClassNotFoundException, SQLException, RefException {
		UnitServerDA usDA = (UnitServerDA) getDataAccessInstance(UnitServerDA.class);
		Vector<UnitServerCuInstance> assoc = usDA.returnAllUnitServerCUAssociationbyUSAlias(unitServerSelected);
		for (UnitServerCuInstance unitServerCuInstance : assoc) {
			usDA.removeAllAttributeConfigurationForAssociation(unitServerCuInstance);
		}
		usDA.deleteUnitServer(unitServerSelected);
		commit();
	}

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
		usList = usDA.getAllUnitServer();
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

	/**
	 * 
	 * @param usci
	 * @throws InstantiationException
	 * @throws IllegalAccessException
	 * @throws ClassNotFoundException
	 * @throws RefException
	 * @throws SQLException
	 */
	public void saveUSCUAssociation(UnitServerCuInstance usci) throws InstantiationException, IllegalAccessException, ClassNotFoundException, RefException, SQLException {
		UnitServerDA usDA = (UnitServerDA) getDataAccessInstance(UnitServerDA.class);
		if(usci.isCuIdModified()) {
			//need to be updated
			usDA.updateUSCUAssociation(usci);
		} else {
			//new creation
			usDA.insertNewUSCUAssociation(usci);
		}
		commit();
	}

	/**
	 * 
	 * @param usci
	 * @return
	 * @throws InstantiationException
	 * @throws IllegalAccessException
	 * @throws ClassNotFoundException
	 * @throws RefException
	 * @throws SQLException
	 */
	public Vector<UnitServerCuInstance> loadAllAssociationForUnitServerAlias(String unitServerAlias) throws InstantiationException, IllegalAccessException, ClassNotFoundException, RefException, SQLException {
		UnitServerDA usDA = (UnitServerDA) getDataAccessInstance(UnitServerDA.class);
		return usDA.returnAllUnitServerCUAssociationbyUSAlias(unitServerAlias);
	}

	/**
	 * 
	 * @param uniqueID
	 * @return
	 * @throws Throwable 
	 */
	public Device getDeviceFromUniqueID(String uniqueID) throws Throwable {
		DeviceDA dDA = (DeviceDA) getDataAccessInstance(DeviceDA.class);
		return dDA.getDeviceFromDeviceIdentification(uniqueID);
	}
	
	/**
	 * 
	 * @param associationToRemove
	 * @throws InstantiationException
	 * @throws IllegalAccessException
	 * @throws ClassNotFoundException
	 * @throws RefException
	 * @throws SQLException
	 */
	public void removeUSCUAssociation(UnitServerCuInstance associationToRemove) throws InstantiationException, IllegalAccessException, ClassNotFoundException, RefException, SQLException {
		UnitServerDA usDA = (UnitServerDA) getDataAccessInstance(UnitServerDA.class);
		usDA.removeAssociation(associationToRemove);
		commit();
	}

	/**
	 * 
	 * @param associationToRemove
	 * @throws InstantiationException
	 * @throws IllegalAccessException
	 * @throws ClassNotFoundException
	 * @throws SQLException
	 * @throws RefException
	 */
	public void removeUSCUAssociation(Collection<UnitServerCuInstance> associationToRemove) throws InstantiationException, IllegalAccessException, ClassNotFoundException, SQLException, RefException {
		for (UnitServerCuInstance unitServerCuInstance : associationToRemove) {
			removeUSCUAssociation(unitServerCuInstance);
		}
	}

	/**
	 * 
	 * @param associationInstance
	 * @return
	 * @throws InstantiationException
	 * @throws IllegalAccessException
	 * @throws ClassNotFoundException
	 * @throws RefException
	 */
	public Vector<DatasetAttribute> loadAllAssociationAttributeConfigForUnitServerAlias(UnitServerCuInstance associationInstance) throws InstantiationException, IllegalAccessException, ClassNotFoundException, RefException {
		UnitServerDA usDA = (UnitServerDA) getDataAccessInstance(UnitServerDA.class);
		return usDA.loadAllAttributeConfigForAssociation(associationInstance);
	}

	/**
	 * 
	 * @param associationInstance
	 * @throws InstantiationException
	 * @throws IllegalAccessException
	 * @throws ClassNotFoundException
	 * @throws RefException
	 * @throws SQLException
	 */
	public void saveAllAttributeConfigForAssociation(UnitServerCuInstance associationInstance) throws InstantiationException, IllegalAccessException, ClassNotFoundException, RefException, SQLException {
		UnitServerDA usDA = (UnitServerDA) getDataAccessInstance(UnitServerDA.class);
		usDA.saveAllAttributeConfigForAssociation(associationInstance);
		commit();
	}

	/**
	 * 
	 * @param string
	 * @param string2
	 * @throws ClassNotFoundException
	 * @throws IllegalAccessException
	 * @throws InstantiationException
	 * @throws SQLException
	 * @throws RefException
	 */
	public void addCuTypeToUnitServerAlias(String unitServerAlias, String cuTypeName) throws RefException, InstantiationException, IllegalAccessException, ClassNotFoundException, SQLException {
		UnitServerDA usDA = (UnitServerDA) getDataAccessInstance(UnitServerDA.class);
		usDA.addCuTypeToUnitServer(unitServerAlias, cuTypeName);
		commit();
	}

	/**
	 * 
	 * @param unitServerSelected
	 * @param cuTypeName
	 * @throws InstantiationException
	 * @throws IllegalAccessException
	 * @throws ClassNotFoundException
	 * @throws RefException
	 * @throws SQLException
	 */
	public void removeCuTypeToUnitServer(String unitServerSelected, String cuTypeName) throws InstantiationException, IllegalAccessException, ClassNotFoundException, RefException, SQLException {
		UnitServerDA usDA = (UnitServerDA) getDataAccessInstance(UnitServerDA.class);
		usDA.removeCuTypeToUnitServer(unitServerSelected, cuTypeName);
		commit();
	}

	public Vector<String> getCuTypeForUnitServer(String unit) throws RefException{
		UnitServerDA usDA = (UnitServerDA)  getDataAccessInstance(UnitServerDA.class);
		return usDA.giveCuTypeForServer(unit);
		
	}
	/**
	 * 
	 * @param eventData
	 * @throws InstantiationException
	 * @throws IllegalAccessException
	 * @throws ClassNotFoundException
	 * @throws RefException
	 * @throws SQLException
	 */
	public void switcAutoloadOptionOnAssociation(Set<UnitServerCuInstance> eventData) throws InstantiationException, IllegalAccessException, ClassNotFoundException, RefException, SQLException {
		for (UnitServerCuInstance unitServerCuInstance : eventData) {
			switcAutoloadOptionOnAssociation(unitServerCuInstance);
		}
	}

	/**
	 * 
	 * @param unitServerCuInstance
	 * @throws InstantiationException
	 * @throws IllegalAccessException
	 * @throws ClassNotFoundException
	 * @throws RefException
	 * @throws SQLException
	 */
	public void switcAutoloadOptionOnAssociation(UnitServerCuInstance unitServerCuInstance) throws InstantiationException, IllegalAccessException, ClassNotFoundException, RefException, SQLException {
		UnitServerDA usDA = (UnitServerDA) getDataAccessInstance(UnitServerDA.class);
		usDA.updateAutoloadForAssociation(unitServerCuInstance, !unitServerCuInstance.getAutoLoad());
		commit();
	}

	
	public void insertNewDeviceClass(DeviceClass dc) throws InstantiationException, IllegalAccessException, ClassNotFoundException, RefException, SQLException{
		UnitServerDA usDA = (UnitServerDA) getDataAccessInstance(UnitServerDA.class);
		usDA.insertNewDeviceClass(dc);
		commit();
	}
	public void removeDeviceClass(String alias) throws RefException, SQLException{
		UnitServerDA usDA = (UnitServerDA) getDataAccessInstance(UnitServerDA.class);
		usDA.removeDeviceClass(alias);
		commit();
	}
	
	public Vector<DeviceClass> returnAllClassesBy(String unit_server, String dc_name, String dc_interface) throws RefException{
		UnitServerDA usDA = (UnitServerDA) getDataAccessInstance(UnitServerDA.class);
		return usDA.returnAllClassesBy(unit_server, dc_name, dc_interface);
	}

}
