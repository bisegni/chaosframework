/**
 * 
 */
package it.infn.chaos.mds.da;

import it.infn.chaos.mds.business.DatasetAttribute;
import it.infn.chaos.mds.business.DeviceClass;
import it.infn.chaos.mds.business.UnitServer;
import it.infn.chaos.mds.business.UnitServerCuInstance;
import it.infn.chaos.mds.secutiry.RSAKey;

import java.io.IOException;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Types;
import java.util.Arrays;
import java.util.Vector;

import org.bouncycastle.crypto.InvalidCipherTextException;
import org.bson.BasicBSONObject;
import org.ref.common.exception.RefException;
import org.ref.common.helper.ExceptionHelper;
import org.ref.server.data.DataAccess;
import org.ref.server.db.sqlbuilder.DeleteSqlBuilder;
import org.ref.server.db.sqlbuilder.InsertUpdateBuilder;
import org.ref.server.db.sqlbuilder.SqlBuilder;
import org.ref.server.db.sqlbuilder.SqlTable;

import com.mongodb.util.JSON;

/**
 * @author bisegni
 * 
 */
public class UnitServerDA extends DataAccess {

	/**
	 * 
	 * @return
	 * @throws Throwable
	 */
	public Vector<UnitServer> getAllUnitServer() throws Throwable {
		PreparedStatement ps = null;
		ResultSet rs = null;
		Vector<UnitServer> result = new Vector<UnitServer>();
		SqlBuilder s = new SqlBuilder();
		s.addTable(UnitServer.class);
		s.addPseudoColumntoSelect("*");
		try {
			ps = getPreparedStatementForSQLCommand(s.toString());
			Vector<?> result_partial = executeQueryPreparedStatementAndClose(ps, UnitServer.class, null, null, false);
			for (Object object : result_partial) {
				UnitServer us = (UnitServer) object;
				fillUnitServerWithCUTypes(us);
				result.addElement(us);
			}
		} catch (IllegalArgumentException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 1, "UnitServerDA::getAlUnitServer");
		} catch (IllegalAccessException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 2, "UnitServerDA::getAlUnitServer");
		} catch (SQLException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 3, "UnitServerDA::getAlUnitServer");
		} finally {
		}
		return result;
	}

	/**
	 * 
	 * @param unitServerAlias
	 * @return
	 * @throws RefException
	 */
	public UnitServer getUnitServerByAlias(String unitServerAlias) throws RefException {
		PreparedStatement ps = null;
		UnitServer result = null;
		SqlBuilder s = new SqlBuilder();
		s.addTable(UnitServer.class);
		s.addPseudoColumntoSelect("*");
		s.addCondition(true, String.format("unit_server_alias='%s'", unitServerAlias));
		try {
			ps = getPreparedStatementForSQLCommand(s.toString());
			result = (UnitServer) executeQuerySingleResultPreparedStatementAndClose(ps, UnitServer.class, null, null, false);
			if (result != null) {
				fillUnitServerWithCUTypes(result);
			}
		} catch (IllegalArgumentException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 1, "UnitServerDA::getAlUnitServer");
		} catch (IllegalAccessException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 2, "UnitServerDA::getAlUnitServer");
		} catch (SQLException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 3, "UnitServerDA::getAlUnitServer");
		} catch (Throwable e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 3, "UnitServerDA::getAlUnitServer");
		} finally {
		}
		return result;
	}

	public void fillUnitServerWithCUTypes(UnitServer us) throws RefException {
		PreparedStatement ps = null;
		ResultSet rs = null;
		SqlBuilder cuTypesSql = new SqlBuilder();
		cuTypesSql.addTable("unit_server_published_cu");
		cuTypesSql.addPseudoColumntoSelect("control_unit_alias");
		cuTypesSql.addCondition(true, String.format("unit_server_alias='%s'", us.getAlias()));

		try {
			ps = getPreparedStatementForSQLCommand(cuTypesSql.toString());
			rs = ps.executeQuery();
			us.getPublischedCU().clear();
			while (rs.next()) {
				us.getPublischedCU().addElement(rs.getString(1));
			}
		} catch (Exception e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 0, "UnitServerDA::getAlUnitServer");
		} finally {
			closeResultSet(rs);
			closePreparedStatement(ps);
		}
	}

	/**
	 * Insert new unit server
	 * 
	 * @param deviceID
	 * @throws RefException
	 * @throws SQLException
	 * @throws IllegalAccessException
	 * @throws IllegalArgumentException
	 */
	public void insertNewUnitServer(UnitServer unitServer) throws RefException {
		PreparedStatement ps = null;
		// generate the key
		try {
			// RSAKeys keys = new RSAKeys();
			// keys.generate();
			// unitServer.setPublic_key(keys.getPublicKeyAsB64String());
			// unitServer.setPrivate_key(keys.getPrivateKeyAsB64String());
			InsertUpdateBuilder iuBuilder = new InsertUpdateBuilder(InsertUpdateBuilder.MODE_INSERT);
			iuBuilder.addTable(UnitServer.class);
			iuBuilder.addColumnAndValue(UnitServer.UNIT_SERVER_HB_TIME, "$CURRENT_TIMESTAMP");

			iuBuilder.fillWithBusinessClass(unitServer);
			ps = getPreparedStatementForSQLCommand(iuBuilder.toString());
			iuBuilder.fillPreparedStatement(ps);
			executeInsertUpdateAndClose(ps);
			
			for (String newType : unitServer.getPublischedCU()) {
				addCuTypeToUnitServer(unitServer.getAlias(), newType);
			}
		} catch (IllegalArgumentException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 0, "UnitServerDA::insertNewUnitServer");
		} catch (IllegalAccessException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 1, "UnitServerDA::insertNewUnitServer");
		} catch (SQLException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 2, "UnitServerDA::insertNewUnitServer");
			// } catch (NoSuchAlgorithmException e) {
			// throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 3, "UnitServerDA::insertNewUnitServer");
			// } catch (NoSuchProviderException e) {
			// throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 4, "UnitServerDA::insertNewUnitServer");
		} finally {
			closePreparedStatement(ps);
		}
	}

	/**
	 * 
	 * @param usci
	 * @throws RefException
	 */
	public void insertNewUSCUAssociation(UnitServerCuInstance usci) throws RefException {
		PreparedStatement ps = null;
		InsertUpdateBuilder iuBuilder = new InsertUpdateBuilder(InsertUpdateBuilder.MODE_INSERT);
		iuBuilder.addTable(UnitServerCuInstance.class);
		try {
			iuBuilder.fillWithBusinessClass(usci);
			ps = getPreparedStatementForSQLCommand(iuBuilder.toString());
			iuBuilder.fillPreparedStatement(ps);
			executeInsertUpdateAndClose(ps);
		} catch (IllegalArgumentException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 0, "UnitServerDA::insertNewUnitServer");
		} catch (IllegalAccessException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 1, "UnitServerDA::insertNewUnitServer");
		} catch (SQLException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 2, "UnitServerDA::insertNewUnitServer");
		} finally {
			closePreparedStatement(ps);
		}
	}

	

	public void updateUSCUAssociation(UnitServerCuInstance unitServerCuInstance) throws RefException {
		PreparedStatement ps = null;
		try {
			InsertUpdateBuilder u = new InsertUpdateBuilder(InsertUpdateBuilder.MODE_UPDATE);
			u.addTable(UnitServerCuInstance.class);
			u.fillWithBusinessClass(unitServerCuInstance);
			u.addCondition(true, String.format("%s=?", UnitServerCuInstance.UNIT_SERVER_ALIAS));
			u.addCondition(true, String.format("%s=?", UnitServerCuInstance.CU_ID));
			ps = getPreparedStatementForSQLCommand(u.toString());
			int idx = u.fillPreparedStatement(ps);
			ps.setString(idx++, unitServerCuInstance.getUnitServerAlias());
			ps.setString(idx++, unitServerCuInstance.getOldCUId());
			executeInsertUpdateAndClose(ps);
		} catch (IllegalArgumentException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 0, "UnitServerDA::updateAutoloadForAssociation");
		} catch (SQLException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 1, "UnitServerDA::updateAutoloadForAssociation");
		} catch (IllegalAccessException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 2, "UnitServerDA::updateAutoloadForAssociation");
		} finally {
			closePreparedStatement(ps);
		}
		
	}
	///////////////////// DEVICE CLASSES /////////////
	/**
	 * 
	 * @param usci
	 * @throws RefException
	 */
	public void insertNewDeviceClass(DeviceClass usci) throws RefException {
		PreparedStatement ps = null;
		InsertUpdateBuilder iuBuilder = new InsertUpdateBuilder(InsertUpdateBuilder.MODE_INSERT);
		iuBuilder.addTable(DeviceClass.class);
		try {
			iuBuilder.fillWithBusinessClass(usci);
			ps = getPreparedStatementForSQLCommand(iuBuilder.toString());
			iuBuilder.fillPreparedStatement(ps);
			executeInsertUpdateAndClose(ps);
		} catch (IllegalArgumentException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 0, "UnitServerDA::insertNewDeviceClass");
		} catch (IllegalAccessException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 1, "UnitServerDA::insertNewDeviceClass");
		} catch (SQLException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 2, "UnitServerDA::insertNewDeviceClass");
		} finally {
			closePreparedStatement(ps);
		}
	}
	
	public void removeDeviceClass(String alias) throws RefException, SQLException {
		// delete the unit server
			DeleteSqlBuilder d = new DeleteSqlBuilder();
			d.addTable(DeviceClass.class);
			d.addCondition(true, String.format("%s=?", DeviceClass.DEVICE_CLASS_ALIAS));
			PreparedStatement ps = getPreparedStatementForSQLCommand(d.toString());
			ps.setString(1, alias);
			executeInsertUpdateAndClose(ps);
	}
	
	public Vector<DeviceClass> returnAllClassesBy(String dev_interface,String dev_class) throws RefException {
		SqlBuilder s = new SqlBuilder();
		Vector<DeviceClass> result = new Vector<DeviceClass>();
		s.addTable(DeviceClass.class);
		s.addPseudoColumntoSelect("*");
		if(dev_interface!=null){
		
			s.addCondition(true,String.format("%s='%s'", DeviceClass.DEVICE_CLASS_INTERFACE,dev_interface));
		}
		
		if(dev_class!=null){

			s.addCondition(true,String.format("%s='%s'", DeviceClass.DEVICE_CLASS_NAME,dev_class));
		}
		PreparedStatement ps = null;
		try {
			ps = getPreparedStatementForSQLCommand(s.toString());
			
			
			result = (Vector<DeviceClass>) executeQueryPreparedStatementAndClose(ps, DeviceClass.class, null, null, false);
			
		} catch (Throwable e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 0, "UnitServerDA::returnAllClassesByInterface");
		}
		return result;
	}
/////////////////////////////////////////	
	/**
	 * Insert new unit server
	 * 
	 * @param deviceID
	 * @throws RefException
	 * @throws SQLException
	 * @throws IllegalAccessException
	 * @throws IllegalArgumentException
	 */
	public void updateUnitServerTSAndIP(UnitServer unitServer) throws RefException {
		InsertUpdateBuilder iuBuilder = new InsertUpdateBuilder(InsertUpdateBuilder.MODE_UPDATE);
		iuBuilder.addTable(UnitServer.class);
		iuBuilder.addColumnAndValue(UnitServer.UNIT_SERVER_HB_TIME, "$CURRENT_TIMESTAMP");
		iuBuilder.addColumnAndValue(UnitServer.UNIT_SERVER_IP_PORT, unitServer.getIp_port());
		iuBuilder.addCondition(true, "unit_server_alias=?");
		
		PreparedStatement ps = null;
		try {
			ps = getPreparedStatementForSQLCommand(iuBuilder.toString());
			int idx = iuBuilder.fillPreparedStatement(ps);
			ps.setString(idx++, unitServer.getAlias());
			executeInsertUpdateAndClose(ps);

			// compare stored cu type with new one and insert only new one
			Vector<String> newCuType = (Vector<String>) unitServer.getPublischedCU().clone();
			unitServer.getPublischedCU().clear();
			fillUnitServerWithCUTypes(unitServer);

			// remove from the passe cu type, all the types already present
			newCuType.removeAll(unitServer.getPublischedCU());

			// insert the new one
			for (String newType : newCuType) {
				addCuTypeToUnitServer(unitServer.getAlias(), newType);
			}
		} catch (SQLException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 0, "UnitServerDA::updateNewUnitServer");
		} finally {
			closePreparedStatement(ps);
		}
	}

	public void updateUnitServerProperty(UnitServer unitServer) throws RefException {
		
		InsertUpdateBuilder iuPublishedCUBuilder = new InsertUpdateBuilder(InsertUpdateBuilder.MODE_UPDATE);
		iuPublishedCUBuilder.addTable("unit_server_published_cu");
		iuPublishedCUBuilder.addColumnAndValue(UnitServer.UNIT_SERVER_ALIAS, unitServer.getAlias());
		iuPublishedCUBuilder.addCondition(true, "unit_server_alias=?");
		
		InsertUpdateBuilder iuCUAssociationBuilder = new InsertUpdateBuilder(InsertUpdateBuilder.MODE_UPDATE);
		iuCUAssociationBuilder.addTable(UnitServerCuInstance.class);
		iuCUAssociationBuilder.addColumnAndValue(UnitServerCuInstance.UNIT_SERVER_ALIAS, unitServer.getAlias());
		iuCUAssociationBuilder.addCondition(true, "unit_server_alias=?");
		
		InsertUpdateBuilder iuBuilder = new InsertUpdateBuilder(InsertUpdateBuilder.MODE_UPDATE);
		iuBuilder.addTable(UnitServer.class);
		iuBuilder.addColumnAndValue(UnitServer.UNIT_SERVER_ALIAS, unitServer.getAlias());
		iuBuilder.addColumnAndValue(UnitServer.PRIVATE_KEY, unitServer.getPrivate_key());
		iuBuilder.addColumnAndValue(UnitServer.PUBLIC_KEY, unitServer.getPublic_key());
		iuBuilder.addCondition(true, "unit_server_alias=?");
		PreparedStatement ps = null;
		try {
			ps=getPreparedStatementForSQLCommand("SET foreign_key_checks = 0");
			executeInsertUpdateAndClose(ps);
			
			ps = getPreparedStatementForSQLCommand(iuPublishedCUBuilder.toString());
			int idx = iuPublishedCUBuilder.fillPreparedStatement(ps);
			ps.setString(idx++, unitServer.isAliasChanged() ? unitServer.getOldAliasOnChange() : unitServer.getAlias());
			executeInsertUpdateAndClose(ps);
			
			ps = getPreparedStatementForSQLCommand(iuCUAssociationBuilder.toString());
			idx = iuCUAssociationBuilder.fillPreparedStatement(ps);
			ps.setString(idx++, unitServer.isAliasChanged() ? unitServer.getOldAliasOnChange() : unitServer.getAlias());
			executeInsertUpdateAndClose(ps);
			
			ps = getPreparedStatementForSQLCommand(iuBuilder.toString());
			idx = iuBuilder.fillPreparedStatement(ps);
			ps.setString(idx++, unitServer.isAliasChanged() ? unitServer.getOldAliasOnChange() : unitServer.getAlias());
			executeInsertUpdateAndClose(ps);
		} catch (SQLException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 0, "UnitServerDA::updateNewUnitServer");
		} finally {
			try {
				ps=getPreparedStatementForSQLCommand("SET foreign_key_checks = 1");
				executeInsertUpdateAndClose(ps);
			} catch (SQLException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}

			closePreparedStatement(ps);
		}
	}

	/**
	 * 
	 * @param unitServer
	 * @throws RefException
	 */
	public void deleteSecurityKeys(UnitServer unitServer) throws RefException {
		InsertUpdateBuilder iuBuilder = new InsertUpdateBuilder(InsertUpdateBuilder.MODE_UPDATE);
		iuBuilder.addTable(UnitServer.class);
		iuBuilder.addColumnAndValue(UnitServer.PRIVATE_KEY, "");
		iuBuilder.addColumnAndValue(UnitServer.PUBLIC_KEY, "");
		iuBuilder.addCondition(true, String.format("%s=?", UnitServer.UNIT_SERVER_ALIAS));
		PreparedStatement ps = null;
		try {
			ps = getPreparedStatementForSQLCommand(iuBuilder.toString());
			int idx = iuBuilder.fillPreparedStatement(ps);
			ps.setString(idx++, unitServer.isAliasChanged() ? unitServer.getOldAliasOnChange() : unitServer.getAlias());
			executeInsertUpdateAndClose(ps);
		} catch (SQLException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 0, "UnitServerDA::deleteSecurityKeys");
		} finally {
			closePreparedStatement(ps);
		}
	}

	public void deleteUnitServer(String unitServerToDelete) throws RefException, SQLException {
		// delete all association
		deletePublishedCU(unitServerToDelete);

		// delete the unit server
		DeleteSqlBuilder d = new DeleteSqlBuilder();
		d.addTable(UnitServer.class);
		d.addCondition(true, String.format("%s=?", UnitServer.UNIT_SERVER_ALIAS));
		PreparedStatement ps = getPreparedStatementForSQLCommand(d.toString());
		ps.setString(1, unitServerToDelete);
		executeInsertUpdateAndClose(ps);
	}

	public boolean unitServerAlreadyRegistered(UnitServer unitServer) throws RefException {
		boolean result = false;
		ResultSet rs = null;
		PreparedStatement ps = null;
		SqlBuilder sb = new SqlBuilder();
		sb.addTable(UnitServer.class);
		sb.addPseudoColumntoSelect("count(*)");
		sb.addCondition(true, String.format("%s = ?", UnitServer.UNIT_SERVER_ALIAS));
		try {
			ps = getPreparedStatementForSQLCommand(sb.toString());
			ps.setString(1, unitServer.getAlias());
			rs = ps.executeQuery();
			if (rs.next()) {
				result = rs.getInt(1) > 0;
			}
		} catch (Exception e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 0, "UnitServerDA::unitServerAlreadyRegistered");
		} finally {
			closeResultSet(rs);
			closePreparedStatement(ps);
		}

		return result;
	}

	public void insertPublishedCU(UnitServer unitServer) throws RefException {
		for (String cuAlias : unitServer.getPublischedCU()) {
			addCuTypeToUnitServer(unitServer.getAlias(), cuAlias);
		}
	}

	public void addCuTypeToUnitServer(String unitServerAlias, String cuTypeName) throws RefException {
		PreparedStatement ps = null;
		InsertUpdateBuilder iuBuilder = null;
		try {
			iuBuilder = new InsertUpdateBuilder(InsertUpdateBuilder.MODE_INSERT);
			iuBuilder.addTable("unit_server_published_cu");
			iuBuilder.addColumnAndValue("unit_server_alias", unitServerAlias);
			iuBuilder.addColumnAndValue("control_unit_alias", cuTypeName);
			ps = getPreparedStatementForSQLCommand(iuBuilder.toString());
			iuBuilder.fillPreparedStatement(ps);
			executeInsertUpdateAndClose(ps);
		} catch (IllegalArgumentException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 0, "UnitServerDA::insertPublishedCU");
		} catch (SQLException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 0, "UnitServerDA::insertPublishedCU");
		} finally {
			closePreparedStatement(ps);
		}
	}

	public void deletePublishedCU(UnitServer unitServer) throws RefException {
		deletePublishedCU(unitServer.getAlias());
	}

	public void deletePublishedCU(String unitServerIdentifier) throws RefException {
		PreparedStatement ps = null;
		DeleteSqlBuilder d = new DeleteSqlBuilder();
		d.addTable("unit_server_published_cu");
		d.addCondition(true, "unit_server_alias = ?");
		try {
			ps = getPreparedStatementForSQLCommand(d.toString());
			ps.setString(1, unitServerIdentifier);
			executeInsertUpdateAndClose(ps);
		} catch (SQLException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 0, "UnitServerDA::deletePublishedCU");
		} finally {
			closePreparedStatement(ps);
		}
	}

	public void removeCuTypeToUnitServer(String unitServerIdentifier, String cuTypeName) throws RefException {
		PreparedStatement ps = null;
		DeleteSqlBuilder d = new DeleteSqlBuilder();
		d.addTable("unit_server_published_cu");
		d.addCondition(true, "unit_server_alias = ?");
		d.addCondition(true, "control_unit_alias = ?");
		try {
			ps = getPreparedStatementForSQLCommand(d.toString());
			ps.setString(1, unitServerIdentifier);
			ps.setString(2, cuTypeName);
			executeInsertUpdateAndClose(ps);
		} catch (SQLException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 0, "UnitServerDA::deletePublishedCU");
		} finally {
			closePreparedStatement(ps);
		}
	}

	public Vector<UnitServerCuInstance> returnAllUnitServerCUAssociationbyUSAlias(String unitServerAlias) throws RefException {
		SqlBuilder s = new SqlBuilder();
		Vector<UnitServerCuInstance> result = new Vector<UnitServerCuInstance>();
		s.addTable(UnitServerCuInstance.class);
		s.addPseudoColumntoSelect("*");
		s.addCondition(true, String.format("%s=?", UnitServerCuInstance.UNIT_SERVER_ALIAS));
		PreparedStatement ps = null;
		try {
			ps = getPreparedStatementForSQLCommand(s.toString());

			ps.setString(1, unitServerAlias);
			result = (Vector<UnitServerCuInstance>) executeQueryPreparedStatementAndClose(ps, UnitServerCuInstance.class, null, null, false);
			for (UnitServerCuInstance unitServerCuInstance : result) {
				BasicBSONObject bdriverDescription = (BasicBSONObject) JSON.parse(unitServerCuInstance.getDrvSpec());
				//System.out.println(bdriverDescription);
			}
		} catch (Throwable e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 0, "UnitServerDA::returnAllUnitServerCUAssociationbyUSAlias");
		}
		return result;
	}

	public Vector<UnitServerCuInstance> loadAllAssociationForUnitServerAliasInAutoload(String unitServerAlias) throws RefException {
		SqlBuilder s = new SqlBuilder();
		Vector<UnitServerCuInstance> result = new Vector<UnitServerCuInstance>();
		s.addTable(UnitServerCuInstance.class);
		s.addPseudoColumntoSelect("*");
		s.addCondition(true, "auto_load='1'");
		s.addCondition(true, String.format("%s=?", UnitServerCuInstance.UNIT_SERVER_ALIAS));
		PreparedStatement ps = null;
		try {
			ps = getPreparedStatementForSQLCommand(s.toString());

			ps.setString(1, unitServerAlias);
			result = (Vector<UnitServerCuInstance>) executeQueryPreparedStatementAndClose(ps, UnitServerCuInstance.class, null, null, false);
			for (UnitServerCuInstance unitServerCuInstance : result) {
				BasicBSONObject bdriverDescription = (BasicBSONObject) JSON.parse(unitServerCuInstance.getDrvSpec());
				//System.out.println(bdriverDescription);
			}
		} catch (Throwable e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 0, "UnitServerDA::returnAllUnitServerCUAssociationbyUSAlias");
		}
		return result;
	}

	/**
	 * 
	 * @param associationToRemove
	 * @throws RefException
	 * @throws SQLException
	 */
	public void removeAssociation(UnitServerCuInstance associationToRemove) throws RefException {
		DeleteSqlBuilder dsb = new DeleteSqlBuilder();
		dsb.addTable(UnitServerCuInstance.class);
		dsb.addCondition(true, String.format("%s = ?", UnitServerCuInstance.UNIT_SERVER_ALIAS));
		dsb.addCondition(true, String.format("%s = ?", UnitServerCuInstance.CU_ID));

		DeleteSqlBuilder deleteConfiguration = new DeleteSqlBuilder();
		deleteConfiguration.addTable("attribute_config");
		deleteConfiguration.addCondition(true, "unique_id = ?");

		PreparedStatement ps;
		try {
			ps = getPreparedStatementForSQLCommand(dsb.toString());
			ps.setString(1, associationToRemove.getUnitServerAlias());
			ps.setString(2, associationToRemove.getCuId());
			executeInsertUpdateAndClose(ps);

			ps = getPreparedStatementForSQLCommand(deleteConfiguration.toString());
			ps.setString(1, associationToRemove.getCuId());
			executeInsertUpdateAndClose(ps);
		} catch (SQLException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 0, "UnitServerDA::removeAssociation");
		}

	}

	/**
	 * 
	 * @param controlUnitInstance
	 * @return
	 * @throws RefException
	 */
	public boolean cuIDIsMDSManaged(String controlUnitInstance) throws RefException {
		SqlBuilder s = new SqlBuilder();
		s.addTable(UnitServerCuInstance.class);
		s.addPseudoColumntoSelect("count(*)");
		s.addCondition(true, String.format("%s = ?", UnitServerCuInstance.CU_ID));
		PreparedStatement ps;
		try {
			ps = getPreparedStatementForSQLCommand(s.toString());
			ps.setString(1, controlUnitInstance);
			return executeCountFromPreparedStatementAndClose(ps, 1) == 1;
		} catch (SQLException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 0, "UnitServerDA::cuIDSelfManageable");
		} catch (Exception e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 0, "UnitServerDA::cuIDSelfManageable");
		}
	}

	public void setState(String deviceIdentification, String state) throws RefException {
		PreparedStatement ps = null;
		InsertUpdateBuilder iuBuilder = new InsertUpdateBuilder(InsertUpdateBuilder.MODE_UPDATE);
		iuBuilder.addTable(UnitServerCuInstance.class);
		iuBuilder.addColumnAndValue(UnitServerCuInstance.STATE, state);
		iuBuilder.addCondition(true, String.format("%s=?", UnitServerCuInstance.CU_ID));
		try {
			ps = getPreparedStatementForSQLCommand(iuBuilder.toString());
			int idx = iuBuilder.fillPreparedStatement(ps);
			ps.setString(idx++, deviceIdentification);
			executeInsertUpdateAndClose(ps);
		} catch (IllegalArgumentException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 0, "UnitServerDA::setState");
		} catch (SQLException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 2, "UnitServerDA::setState");
		} finally {
			closePreparedStatement(ps);
		}
	}

	/**
	 * 
	 * @param associationInstance
	 * @return
	 * @throws RefException
	 */
	public Vector<DatasetAttribute> loadAllAttributeConfigForAssociation(UnitServerCuInstance associationInstance) throws RefException {
		return loadAllAttributeConfigForAssociation(associationInstance.getCuId());
	}

	/**
	 * 
	 * @param cuID
	 * @return
	 * @throws RefException
	 */
	public Vector<DatasetAttribute> loadAllAttributeConfigForAssociation(String cuID) throws RefException {
		Vector<DatasetAttribute> result = new Vector<DatasetAttribute>();
		PreparedStatement ps = null;
		ResultSet rs = null;
		SqlBuilder s = new SqlBuilder();
		s.addPseudoColumntoSelect("*");
		s.addTable("attribute_config");
		s.addCondition(true, String.format("%s=?", "unique_id"));
		try {
			ps = getPreparedStatementForSQLCommand(s.toString());
			ps.setString(1, cuID);
			rs = ps.executeQuery();
			while (rs.next()) {
				DatasetAttribute da = new DatasetAttribute();
				da.setName(rs.getString("attribute_name"));
				da.setDefaultValueNoCheck(rs.getString("default_value"));
				da.setRangeMaxNoCheck(rs.getString("max_value"));
				da.setRangeMinNoCheck(rs.getString("min_value"));
				result.add(da);
			}
		} catch (IllegalArgumentException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 0, "UnitServerDA::loadAllAttributeConfigForAssociation");
		} catch (SQLException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 2, "UnitServerDA::loadAllAttributeConfigForAssociation");
		} finally {
			closePreparedStatement(ps);
		}

		return result;
	}

	public void removeAllAttributeConfigurationForAssociation(UnitServerCuInstance associationInstance) throws RefException {
		// first delete all config for device id
		PreparedStatement ps = null;
		try {
			DeleteSqlBuilder d = new DeleteSqlBuilder();
			d.addTable("attribute_config");
			d.addCondition(true, String.format("%s=?", "unique_id"));
			ps = getPreparedStatementForSQLCommand(d.toString());
			ps.setString(1, associationInstance.getCuId());
			executeInsertUpdateAndClose(ps);
		} catch (IllegalArgumentException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 0, "UnitServerDA::setState");
		} catch (SQLException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 2, "UnitServerDA::setState");
		} finally {
			closePreparedStatement(ps);
		}
	}

	/**
	 * 
	 * @param associationInstance
	 * @throws RefException
	 */
	public void saveAllAttributeConfigForAssociation(UnitServerCuInstance associationInstance) throws RefException {
		PreparedStatement ps = null;
		try {
			// first delete all config for device id
			DeleteSqlBuilder d = new DeleteSqlBuilder();
			d.addTable("attribute_config");
			d.addCondition(true, String.format("%s=?", "unique_id"));
			ps = getPreparedStatementForSQLCommand(d.toString());
			ps.setString(1, associationInstance.getCuId());
			executeInsertUpdateAndClose(ps);

			// write all configuration
			InsertUpdateBuilder i = new InsertUpdateBuilder();
			i.addTable("attribute_config");
			i.addColumnAndValue("unique_id", associationInstance.getCuId());

			Vector<DatasetAttribute> conf = associationInstance.getAttributeConfigutaion();
			for (DatasetAttribute datasetAttribute : conf) {
				i.addColumnAndValue("attribute_name", datasetAttribute.getName());
				if (datasetAttribute.getDefaultValue() != null && datasetAttribute.getDefaultValue().length() > 0)
					i.addColumnAndValue("default_value", datasetAttribute.getDefaultValue());
				else
					i.addNullColumnAndType("default_value", Types.VARCHAR);

				if (datasetAttribute.getRangeMax() != null && datasetAttribute.getRangeMax().length() > 0)
					i.addColumnAndValue("max_value", datasetAttribute.getRangeMax());
				else
					i.addNullColumnAndType("max_value", Types.VARCHAR);

				if (datasetAttribute.getRangeMin() != null && datasetAttribute.getRangeMin().length() > 0)
					i.addColumnAndValue("min_value", datasetAttribute.getRangeMin());
				else
					i.addNullColumnAndType("min_value", Types.VARCHAR);

				ps = getPreparedStatementForInputUpdateBuilder(i);

				executeInsertUpdateAndClose(ps);
			}
		} catch (IllegalArgumentException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 0, "UnitServerDA::setState");
		} catch (SQLException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 2, "UnitServerDA::setState");
		} finally {
			closePreparedStatement(ps);
		}
	}

	/**
	 * 
	 * @param cuID
	 * @param attributes
	 * @throws RefException
	 */
	public void configuraDataseAttributestForCUID(String cuID, Vector<DatasetAttribute> attributes) throws RefException {
		Vector<DatasetAttribute> configuredAttribute = loadAllAttributeConfigForAssociation(cuID);

		for (DatasetAttribute configuredAttirbute : configuredAttribute) {
			// iterate the input attributes to find the corespond one
			for (DatasetAttribute inputAttribute : attributes) {
				if (configuredAttirbute.getName().toUpperCase().equals(inputAttribute.getName().toUpperCase())) {
					try {
						// is the same attribute so try so the the configured value
						if (configuredAttirbute.getDefaultValue() != null)
							inputAttribute.setDefaultValue(configuredAttirbute.getDefaultValue());
						if (configuredAttirbute.getRangeMax() != null)
							inputAttribute.setRangeMax(configuredAttirbute.getRangeMax());
						if (configuredAttirbute.getRangeMin() != null)
							inputAttribute.setRangeMin(configuredAttirbute.getRangeMin());
					} catch (RefException e) {
						System.out.println(ExceptionHelper.getInstance().putExcetpionStackToString(e));
					}

				}
			}
		}

	}

	public void updateAutoloadForAssociation(UnitServerCuInstance unitServerCuInstance, boolean autoLoadFlag) throws RefException {
		PreparedStatement ps = null;
		try {
			InsertUpdateBuilder u = new InsertUpdateBuilder(InsertUpdateBuilder.MODE_UPDATE);
			u.addTable(UnitServerCuInstance.class);
			u.addColumnAndValue(UnitServerCuInstance.AUTO_LOAD, autoLoadFlag);
			u.addCondition(true, String.format("%s=?", UnitServerCuInstance.UNIT_SERVER_ALIAS));
			u.addCondition(true, String.format("%s=?", UnitServerCuInstance.CU_ID));
			ps = getPreparedStatementForSQLCommand(u.toString());
			int idx = u.fillPreparedStatement(ps);
			ps.setString(idx++, unitServerCuInstance.getUnitServerAlias());
			ps.setString(idx++, unitServerCuInstance.getCuId());
			executeInsertUpdateAndClose(ps);
		} catch (IllegalArgumentException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 0, "UnitServerDA::updateAutoloadForAssociation");
		} catch (SQLException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 2, "UnitServerDA::updateAutoloadForAssociation");
		} finally {
			closePreparedStatement(ps);
		}
	}

	public boolean checkPublicKey(String unitServerAlias, String publicKeyBase64) throws RefException {
		boolean result = false;
		ResultSet rs = null;
		PreparedStatement ps = null;
		try {
			SqlBuilder sForKey = new SqlBuilder();
			SqlTable t = sForKey.addTable(UnitServer.class);
			sForKey.addTableColumnToSelect(t.getTableName(), UnitServer.PRIVATE_KEY);
			sForKey.addCondition(true, String.format("%s=?", UnitServer.UNIT_SERVER_ALIAS));
			ps = getPreparedStatementForSQLCommand(sForKey.toString());
			ps.setString(1, unitServerAlias);
			rs = ps.executeQuery();
			if (rs.next()) {
				String privateKeyBase64 = rs.getString(1);
				if (privateKeyBase64 != null && privateKeyBase64.length() > 0) {
					if (publicKeyBase64 != null && publicKeyBase64.length() > 0) {
						RSAKey privateKey = new RSAKey(privateKeyBase64, false);
						RSAKey publicKey = new RSAKey(publicKeyBase64, true);
						// try to encode with private key
						byte[] privEncoding = privateKey.encriptByte(unitServerAlias.getBytes());

						// now decode with the public
						byte[] publicDecoding = publicKey.decriptByte(privEncoding);

						result = Arrays.equals(unitServerAlias.getBytes(), publicDecoding);
					}
				} else {
					result = true;
				}
			} else {
				result = true;
			}
		} catch (IllegalArgumentException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 0, "UnitServerDA::updateAutoloadForAssociation");
		} catch (SQLException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 2, "UnitServerDA::updateAutoloadForAssociation");
		} catch (IOException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 2, "UnitServerDA::updateAutoloadForAssociation");
		} catch (InvalidCipherTextException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 2, "UnitServerDA::updateAutoloadForAssociation");
		} finally {
			closeResultSet(rs);
			closePreparedStatement(ps);
		}

		return result;
	}


}
